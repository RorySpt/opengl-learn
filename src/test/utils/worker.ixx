module;
#include <chrono>
#include <deque>
#include <future>
#include <iostream>
#include <map>
#include <memory>
export module worker;

/*
 * active_worker is a thread, that designed can be add the work, and then consume it automatic in an internal thread.
 *
 * passive_worker is passive, that means need consume it manually on the required thread. With it, you can submit tasks (short)
 * to any thread without locking the contents of the operation.
 */

namespace utils
{

	template<typename Func>
	concept NormalWorkFunction = std::is_invocable_v<Func>;

	template<typename ClassType, auto f >
	concept MemberWorkFunctionClass = std::is_invocable_v<decltype(f), ClassType>
		&& std::is_member_function_pointer_v<decltype(f)>;


	struct worker_base
	{
		using work_t = std::move_only_function<void()>;

		size_t size() const { return _works.size(); }
		bool empty() const { return _works.empty(); }

		template<NormalWorkFunction Func>
		std::future<std::invoke_result_t<Func>> add_work(Func&& work)
		{
			using ret_t = std::invoke_result_t<Func>;

			std::promise<ret_t> promise;
			auto future = promise.get_future();
			std::lock_guard guard(_mutex);
			_works.emplace_back([=, promise = std::move(promise)]()mutable
				{
					if constexpr (std::is_same_v<ret_t, void>)
						(work(), promise.set_value());
					else
						promise.set_value(work());
				});
			_cv.notify_all();
			return future;
		}


		template<auto f, MemberWorkFunctionClass<f> ClassType>
		std::future<std::invoke_result_t<decltype(f), ClassType>> add_work(ClassType* obj)
		{
			using ret_t = std::invoke_result_t<decltype(f), ClassType>;
			std::promise<ret_t> promise;
			auto future = promise.get_future();
			std::lock_guard guard(_mutex);
			_works.emplace_back([=, promise = std::move(promise)]()mutable
				{
					if constexpr (std::is_same_v<ret_t, void>)
						(std::invoke(f, obj), promise.set_value());
					else
						promise.set_value(std::invoke(f, obj));
				});
			return future;
		}

	protected:
		//worker_base() = default;

		std::mutex _mutex;
		std::condition_variable _cv;
		std::deque<work_t> _works;
	};

	export struct passive_worker : worker_base
	{
		
		size_t consume_all()
		{
			std::deque<work_t> works;
			{
				std::lock_guard guard(_mutex);
				if (_works.empty())return 0;
				works.swap(_works);
			}
			

			for (auto& work : works)
				std::invoke(work);
			_cv.notify_all();

			return works.size();
		}
		bool try_consume_one()
		{
			work_t work;
			{
				std::lock_guard guard(_mutex);
				if (_works.empty()) return false;
				work = std::move(_works.front());
				_works.pop_front();
			}
			std::invoke(work);
			_cv.notify_all();
			return true;
		}
	};

	export struct active_worker : passive_worker
	{

		void wait_for_done()
		{
			std::unique_lock locker(_work_mutex);
			_cv.wait(locker
				, [this] {
					return _works.empty();
				});
		}
	private:

		void self_driven(const std::stop_token& stop_token)
		{
			while (!stop_token.stop_requested())
			{
				{
					std::unique_lock locker(_work_mutex);
					_cv.wait(locker
						, [this] {
							return !_works.empty();
						});
				}
				consume_all();
			}
		}

		std::jthread thread = std::jthread(std::bind_front(&active_worker::self_driven, this));

		std::mutex _work_mutex;
	};

	// make passive_worker as the default worker
	using worker = passive_worker;

	struct my_struct
	{
		void f()
		{
			std::cout << __FUNCTION__ << '\n';
		}
	};



	struct task_graph
	{
		enum enum_task
		{
			main_thread,

		};


		std::shared_ptr<passive_worker> get_worker(std::string_view name)
		{
			std::lock_guard guard(mutex);
			std::shared_ptr<passive_worker> worker;
			if (const auto it = worker_map.find(name.data()); it != worker_map.end())
				worker = it->second;
			else
				(worker = std::make_shared<passive_worker>(), worker_map.emplace(name, worker));
			return worker;
		}

		template<NormalWorkFunction Func>
		std::future<std::invoke_result_t<Func>> async(std::string_view name, Func&& work)
		{
			return get_worker(name)->add_work(std::forward<Func>(work));
		}

		template<auto f, MemberWorkFunctionClass<f> ClassType>
		std::future<std::invoke_result_t<decltype(f), ClassType>> async(std::string_view name, ClassType* obj)
		{
			return get_worker(name)->add_work<f>(obj);
		}

		static task_graph* inst()
		{
			static task_graph graph;
			return &graph;
		}
	private:
		std::mutex mutex;
		std::map<std::string, std::shared_ptr<passive_worker>> worker_map;
	};


	struct msg_loop_printer
	{
		void loop(const std::stop_token& stop_token)
		{
			while (!stop_token.stop_requested())
			{
				worker.consume_all();

				std::cout << "[MSG]" + msg << '\n';
			}
		}

		std::jthread thread = std::jthread(std::bind_front(&msg_loop_printer::loop, this));
		std::string msg;
		passive_worker worker ;
	};


	export void worker_test()
	{
		active_worker worker;

		std::mutex mutex;
		std::unique_lock locker(mutex);
		my_struct struct_;
		worker.add_work<&my_struct::f>(&struct_);
		worker.add_work<&my_struct::f>(&struct_);
		worker.add_work<&my_struct::f>(&struct_);
		worker.add_work<&my_struct::f>(&struct_);
		worker.add_work<&my_struct::f>(&struct_);
		worker.wait_for_done();

		msg_loop_printer printer;
		while (true)
		{
			printer.worker.add_work([&]()
				{
					printer.msg.clear();
					std::this_thread::yield();
					printer.msg = std::format("[{:L%F %H:%M:%S}]", std::chrono::zoned_time{
					std::chrono::current_zone(),
					std::chrono::system_clock::now()
						});
				});
			

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

	}
}
