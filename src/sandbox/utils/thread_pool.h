#pragma once
#include <deque>
#include <functional>
#include <future>
#include <type_traits>
#include <thread>

#include "synced_stream.h"

namespace details
{
	template<typename  Callable, typename ...Args> requires std::is_invocable_v<Callable, Args...>
	struct packaged_invoke
	{
		using ReturnType = std::invoke_result_t<Callable, Args...>;

		auto get_future() -> std::future<ReturnType>
		{
			return promise.get_future();
		}
		auto get_value() -> ReturnType
		{
			return get_future().get();
		}
		void operator()() const
		{
			function();
		}

		std::function<void()> function;
		std::promise<ReturnType> promise;
	};


	template<typename Callable, typename ...Args>
		requires std::is_invocable_v<Callable, Args...>
	auto make_package(Callable&& func, Args&& ...args)
	{
		using ReturnType = std::invoke_result_t<Callable, Args...>;
		packaged_invoke<Callable, Args...> packaged;
		packaged.function = [=, &promise = packaged.promise]() mutable
			{

				if constexpr (!std::is_same_v<ReturnType, void>)
				{
					static_cast<Callable&&>(func);
					promise.set_value(std::invoke_r<ReturnType>(func, std::forward<Args>(args)...));
				}
				else
				{
					std::invoke(func, std::forward<Args>(args)...);
					promise.set_value();
				}
			};
		return packaged;
	}

}


class ThreadPool   // NOLINT(cppcoreguidelines-special-member-functions)
{
public:
	ThreadPool(std::size_t n = std::thread::hardware_concurrency())
		:_thread_count(n)
	{
		_initialize_threads();
	}
	~ThreadPool()
	{
		_destroy_threads();
	}

	template<typename  Callable, typename ...Args> requires std::is_invocable_v<Callable, Args...>
	auto submit(Callable&& func, Args&& ...args) -> std::future<std::invoke_result_t<Callable, Args...>>
	{
		using ReturnType = std::invoke_result_t<Callable, Args...>;
		auto promise = std::make_shared<std::promise<ReturnType>>();
		auto future = promise->get_future();
		std::lock_guard guard(_task_mutex);
		_task_deq.emplace_back([=]()mutable
			{
				if constexpr (!std::is_same_v<ReturnType, void>)
				{
					promise->set_value(std::invoke_r<ReturnType>(func, std::forward<Args>(args)...));
				}
				else
				{
					std::invoke(func, std::forward<Args>(args)...);
					promise->set_value();
				}
			});
		cv.notify_one();
		return future;
	}
	size_t get_thread_count() const { return _thread_count; }
private:

	void _initialize_threads()
	{
		_threads.resize(_thread_count);
		for (auto& thread : _threads)
		{
			thread = std::jthread([this](const std::stop_token& stop_token)
				{
					for (;;)
					{
						std::unique_lock unique_lock(_task_mutex);
						cv.wait(unique_lock, [&] {
							return !_task_deq.empty() || stop_token.stop_requested();
							});
						if (stop_token.stop_requested()) return;

						auto task = std::move(_task_deq.back());
						_task_deq.pop_back();
						unique_lock.unlock();
						task();
					}
				});
		}
	}

	void _destroy_threads()
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

	std::condition_variable cv;
	std::mutex _task_mutex;
	std::deque<std::function<void()>> _task_deq;
	std::vector<std::jthread> _threads;
	std::size_t _thread_count;
};


inline void thread_pool_unit_test()
{
	ThreadPool pool;
	std::vector<std::future<int>> futures;
	for (int i = 0; i < pool.get_thread_count(); ++i)
		futures.emplace_back(pool.submit([=]()
			{
				const auto tp = std::chrono::high_resolution_clock::now();
				int count = 0;
				while (std::chrono::high_resolution_clock::now() - tp < std::chrono::seconds(2))
				{
					sync::println("I'm thread {}", i);
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
					//std::this_thread::yield();
					++count;
				}
				return count;
			}));

	for (auto& future : futures)
	{
		future.wait();
	}
	sync::print("result: ");
	for (auto& future : futures)
	{
		sync::print("{} ", future.get());
	}
	sync::println("");
}


