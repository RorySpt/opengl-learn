module;
#include <deque>
#include <functional>
#include <future>
#include <thread>
export module thread_pool;

import synced_stream;



export namespace utils
{
	
	class thread_pool   // NOLINT(cppcoreguidelines-special-member-functions)
	{
	public:
		using task_t = std::move_only_function<void()>;


		thread_pool(std::size_t n = std::thread::hardware_concurrency())
			:_thread_count(n)
		{
			initialize_threads();
		}
		~thread_pool()
		{
			destroy_threads();
		}

		template<typename  Func, typename ...Args> requires std::is_invocable_v<Func, Args...>
		auto submit(Func&& func, Args&& ...args) -> std::future<std::invoke_result_t<Func, Args...>>
		{
			using ret_t = std::invoke_result_t<Func, Args...>;

			std::promise<ret_t> promise;
			auto future = promise.get_future();
			std::lock_guard guard(_task_mutex);
			_task_deq.emplace_back([=, promise = std::move(promise)]() mutable 
				{
					if constexpr (std::is_same_v<ret_t, void>)
					{
						std::invoke(func, std::forward<Args>(args)...);
						promise.set_value();
					}
					else
					{
						promise.set_value(std::invoke_r<ret_t>(func, std::forward<Args>(args)...));
					}
				});
			cv.notify_one();
			return future;
		}
		[[nodiscard]] size_t get_thread_count() const { return _thread_count; }
		[[nodiscard]] size_t get_active_count() const { return _active_count; }

		void wait_for_done()
		{
			std::unique_lock unique_lock(_task_mutex);
			cv_wait_done.wait(unique_lock, [this] {
				return get_active_count() == 0 && _task_deq.empty();
				});
		}
		void resize(std::size_t n = std::thread::hardware_concurrency())
		{
			destroy_threads();
			_thread_count = n;
			initialize_threads();
		}
	private:

		void initialize_threads()
		{
			_threads.resize(_thread_count);
			for (auto& thread : _threads)
			{
				thread = std::jthread([this](const std::stop_token& stop_token)
					{
						while (!stop_token.stop_requested())
						{
							task_t task;
							{
								std::unique_lock unique_lock(_task_mutex);
								cv.wait(unique_lock, [&] {
									return !_task_deq.empty() || stop_token.stop_requested();
									});
								if (stop_token.stop_requested()) return;

								task = std::move(_task_deq.front());
								_task_deq.pop_front();
								++_active_count;
							}
							task();
							--_active_count;
							cv_wait_done.notify_all();
						}
					});
			}
		}

		void destroy_threads()
		{
			for (auto& thread : _threads)
			{
				thread.request_stop();
			}
			cv.notify_all();
			for (auto& thread : _threads)
			{
				thread.join();
			}
		}

		std::condition_variable cv_wait_done;
		std::condition_variable cv;
		std::mutex _task_mutex;
		std::deque<task_t> _task_deq;
		std::vector<std::jthread> _threads;
		std::size_t _thread_count;
		std::atomic_uint32_t _active_count = 0;
	};

	void thread_pool_unit_test()
	{
		thread_pool pool;
		std::vector<std::future<int>> futures;
		for (int i = 0; i < pool.get_thread_count(); ++i)
			futures.emplace_back(pool.submit([=]()
				{
					const auto tp = std::chrono::high_resolution_clock::now();
					int count = 0;
					while (std::chrono::high_resolution_clock::now() - tp < std::chrono::seconds(2))
					{
						sync::println("I'm thread {}", i);
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
						//std::this_thread::yield();
						++count;
					}
					return count;
				}));

		pool.wait_for_done();

		sync::print("result: ");
		for (auto& future : futures)
		{
			sync::print("{} ", future.get());
		}
		sync::println("");


	}
}



