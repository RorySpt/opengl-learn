#include "delegate.h"
#include <algorithm>
#include <execution>

#include "utils/synced_stream.h"


void func(int a, float b)
{
    sync::println("{}: int({}), float({})", __FUNCTION__, a, b);
}
struct A
{
    void func(int a, float b)
    {
        sync::println("{}: int({}), float({})", __FUNCTION__, a, b);
    }
};
void delegate_unit_test()
{
    Delegate_MultiCast<int, float> delegate;

    auto lambda = [](int a, float b)
	    {
            sync::println("{}: int({}), float({})", __FUNCTION__, a, b);
	    };

    A a;
    static_assert(std::is_invocable_v<decltype(&A::func), A*, int, float>);

    std::invoke(&A::func, &a, 1, 2.1f);
    auto id0 = delegate.bind(lambda);
    auto id1 = delegate.bind(func);
    auto id2 = delegate.bind(&a, &A::func);

    delegate.cast(1, 2.1f);

    sync::println("{:*^60}", 1);

    int aaa = 0;
    delegate.unbind(id2);
    delegate.cast(aaa,9.2f);
    delegate.unbind(id0);
    delegate.unbind(id1);
    sync::println("{:*^60}", 2);
    Delegate_SingleCast<void, int, float> single_delegate;

    static_assert(std::is_invocable_r_v<void, decltype(&A::func), A*, int, float>);
    single_delegate.bind(lambda);
    single_delegate.cast(11, 2.1f);
    single_delegate.bind(func);
    single_delegate.cast(22, 5.1f);

    sync::println("{:*^60}", 3);
    auto call_by_thread = [](int a, float b)
        {
            sync::print("thread call: ");
            func(a, b);
        };
    Delegate_Object<int,float> handle{ call_by_thread };

	bool thread_run = true;
    std::thread thread([&thread_run, &handle]()
        {
	        while (thread_run)
	        {
                handle.tick();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
	        }
        });

    handle.feed_for_call(44, 10.1f);
    handle.feed_for_call(88, 10.1f);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    thread_run = false;
    thread.join();


    sync::println("{:*^60}", 4);
    sync::println("begin a Async bind");
    auto multi_cast_handle0 = delegate.bind(lambda, ExecutionPolicy::Async);

    delegate.cast(33, 5.1f);
    sync::println("wait tick...");
    multi_cast_handle0.tick();
    multi_cast_handle0.unbind();

    sync::println("begin a another Async bind");
    auto multi_cast_handle1 = delegate.bind(call_by_thread, ExecutionPolicy::Async);
    delegate.cast(44, 5.1f);
    sync::println("wait tick...");
    sync::println("start a thread for tick");
    thread_run = true;
    thread = std::thread([&thread_run, &multi_cast_handle1]()
        {
            while (thread_run)
            {
                multi_cast_handle1.tick();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

    delegate.cast(111, 5.1f); sync::println("wait tick...");
    delegate.cast(112, 5.1f); sync::println("wait tick...");

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sync::println("stop the thread");
    thread_run = false;
    thread.join();

    delegate.cast(111, 5.1f); sync::println("wait tick...(never tick)");

}
