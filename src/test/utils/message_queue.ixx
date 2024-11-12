module;
#include <functional>
#include <future>
#include <map>
#include <mutex>
#include <print>
#include <queue>
export module message_queue;

import signals;

export namespace utils
{
	struct timer
	{
		using clock = std::chrono::high_resolution_clock;
		static auto system_now()
		{
			return std::chrono::high_resolution_clock::now();
		}

		auto peek() const
		{
			return system_now() - tp;
		}
		auto get()
		{
			const auto last_tp = std::exchange(tp, system_now());
			return tp - last_tp;
		}
		auto reset()
		{
			tp = std::chrono::high_resolution_clock::now();
		}

		clock::time_point tp = std::chrono::high_resolution_clock::now();
	};

	using namespace std::literals::chrono_literals;

	template<typename ...Args>
	struct message_queue
	{
		using connection = typename signal<Args...>::connection;
		
		void emit(Args&&... args)
		{
			std::lock_guard guard(_mutex);
			_queue.push(std::move(args)...);
		}

		template<typename Func> requires std::is_invocable_v<Func, Args...>
		connection add_consumer(Func&& f)
		{
			return _signal.connect(std::move(f));
		}
		size_t remove_consumer(connection c)
		{
			return _signal.disconnect(c);
		}

		void tick(std::chrono::milliseconds threshold = 10ms)
		{
			const timer timer0;
			while(!_queue.empty() && timer0.peek() < threshold)
			{
				_mutex.lock();
				auto args = std::move(_queue.front());
				_queue.pop();
				_mutex.unlock();
				consume(args, std::index_sequence_for<Args...>{});
			}
		}

		size_t size()const { return _queue.size(); }
		bool empty()const { return _queue.empty(); }
	private:
		//void consume_helper(std::tuple<Args...>& args) // thread unsafe;
		//{
		//	consume_helper(args, std::index_sequence_for<Args...>{});
		//}
		template <typename T, T... I>
		void consume(std::tuple<Args...>& args, std::integer_sequence<T, I...>)
		{
			_signal.emit(std::move(std::get<I>(args))...);
		}
		std::mutex _mutex;
		signal<Args...> _signal;
		std::queue<std::tuple<Args...>> _queue;
	};
}

import synced_stream;

int foo(std::string_view s)
{
	static int i = 0;
	sync::println(R"(thread_id: {}, {}, s: "{}")", std::this_thread::get_id(), __FUNCTION__, s);
	return i++;
}

export void mq_test()
{
	using namespace std::literals::chrono_literals;
	utils::message_queue<std::string> mq;

	std::jthread tread0([&](const std::stop_token& stop_token)
		{
			// target thread
			mq.add_consumer([](std::string_view s)
			{
				sync::println(R"(thread_id: {}, {}, s: "{}, ")", std::this_thread::get_id(), __FUNCTION__, s);
				std::this_thread::sleep_for(1ms);
			});
			mq.add_consumer(&foo);
			while (!stop_token.stop_requested())
			{
				mq.tick(); // consume message by consumer in current thread
				// do_something
			}
		});

	sync::println("main thread_id: {}", std::this_thread::get_id());
	// other thread produce the message
	mq.emit("hello, world!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); std::this_thread::yield();

	auto future = std::async([&]()
	{
		sync::println("async thread_id: {}", std::this_thread::get_id());
		mq.emit("hello");
		mq.emit("hello1");
		mq.emit("hello2");
		mq.emit("hello3");
	});
	future.get();
	
	sync::println("wait!!");
	while (!mq.empty())
	{
		std::this_thread::yield();
	}
	sync::println("complete!!");

	tread0.request_stop();
	tread0.join();
}