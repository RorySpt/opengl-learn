#pragma once
//#include <deque>
//#include <functional>
//#include <future>
//#include <type_traits>
//#include <thread>
//
//#ifdef ENABLE_TEST
//#include "synced_stream.h"
//#endif
//
//template<typename T>
//concept ThreadPoolCallable = std::is_invocable_v<T>;
//
//class ThreadPool
//{
//public:
//	using task_t = std::function<void()>;
//
//	ThreadPool(std::size_t n = std::thread::hardware_concurrency())
//		:_thread_count(n)
//	{
//		_initialize_threads();
//	}
//	~ThreadPool()
//	{
//		_destroy_threads();
//	}
//	ThreadPool(const ThreadPool& other) = delete;
//	ThreadPool& operator=(const ThreadPool& other) = delete;
//	ThreadPool(ThreadPool&& other) noexcept = delete;
//	ThreadPool& operator=(ThreadPool&& other) noexcept = delete;
//
//	template<ThreadPoolCallable callable_t>
//	auto submit(callable_t&& func) -> std::future<std::invoke_result_t<callable_t>>
//	{
//		using return_t = std::invoke_result_t<callable_t>;
//		//std::packaged_task<return_t()> task(std::forward<callable_t>(func));
//		//auto future = task.get_future();
//		//{
//		//	std::lock_guard guard(_task_mutex);
//		//	_task_deq.emplace_back(std::move(task));
//		//}
//		
//
//		auto promise = std::make_shared<std::promise<return_t>>();
//		auto future = promise->get_future();
//		std::lock_guard guard(_task_mutex);
//		_task_deq.emplace_back([=]()mutable
//			{
//				if constexpr (!std::is_same_v<return_t, void>) {
//					promise->set_value(std::invoke_r<return_t>(func));
//				}
//				else {
//					std::invoke(func);
//					promise->set_value();
//				}
//			});
//		cv.notify_one();
//		return future;
//	}
//
//	[[nodiscard]] size_t get_thread_count() const { return _thread_count; }
//	[[nodiscard]] size_t get_active_count() const { return _active_count; }
//
//	void wait_for_done()
//	{
//		std::unique_lock unique_lock(_task_mutex);
//		cv_wait_done.wait(unique_lock, [this] {
//			return get_active_count() == 0;
//			});
//	}
//private:
//
//	void _initialize_threads()
//	{
//		_threads.resize(_thread_count);
//		for (auto& thread : _threads)
//		{
//			thread = std::jthread([this](const std::stop_token& stop_token)
//				{
//					for (;;)
//					{
//						task_t task;
//						{
//							std::unique_lock unique_lock(_task_mutex);
//							cv.wait(unique_lock, [&] {
//								return !_task_deq.empty() || stop_token.stop_requested();
//								});
//							if (stop_token.stop_requested()) return;
//							task = std::move(_task_deq.back());
//							_task_deq.pop_back();
//						}
//						++_active_count;
//						std::invoke(task);
//						--_active_count;
//					}
//				});
//		}
//	}
//
//	void _destroy_threads()
//	{
//		for (auto& thread : _threads)
//		{
//			thread.request_stop();
//		}
//		cv.notify_all();
//		for (auto& thread : _threads)
//		{
//			thread.join();
//		}
//	}
//	std::condition_variable cv_wait_done;
//	std::condition_variable cv;
//	std::mutex _task_mutex;
//	std::deque<task_t> _task_deq;
//	std::vector<std::jthread> _threads;
//	std::size_t _thread_count;
//	std::atomic_uint32_t _active_count = 0;
//};
//
//#ifdef ENABLE_TEST
//inline void thread_pool_unit_test()
//{
//	ThreadPool pool;
//	std::vector<std::future<int>> futures;
//	for (std::size_t i = 0; i < pool.get_thread_count(); ++i)
//		futures.emplace_back(pool.submit([=]()
//			{
//				using clock = std::chrono::high_resolution_clock;
//				const auto tp = clock::now();
//				int count = 0;
//				while (clock::now() - tp < std::chrono::seconds(2))
//				{
//					sync::println("I'm thread {}", i);
//					std::this_thread::yield();
//					++count;
//				}
//				return count;
//			}));
//
//	for (auto& future : futures)
//	{
//		future.wait();
//	}
//	sync::print("result: ");
//	for (auto& future : futures)
//	{
//		sync::print("{} ", future.get());
//	}
//	sync::println("");
//}
//#endif

