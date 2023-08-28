#include "delegate.h"
#include "common.h"

using namespace comm;

void func(int a, float b)
{
    println("{}: int({}), float({})", __FUNCTION__, a, b);
}
struct A
{
    void func(int a, float b)
    {
        println("{}: int({}), float({})", __FUNCTION__, a, b);
    }
};
void delegate_unit_test()
{
    Delegate_MultiCast<int, float> delegate;

    auto lambda = [](int a, float b)
    {
        println("{}: int({}), float({})", __FUNCTION__, a, b);
    };

    A a;
    static_assert(std::is_invocable_v<decltype(&A::func), A*, int, float>);

    std::invoke(&A::func, &a, 1, 2.1f);
    auto id0 = delegate.bind(lambda);
    auto id1 = delegate.bind(func);
    auto id2 = delegate.bind(&a, &A::func);

    delegate.cast(1, 2.1f);

    println("{:*^60}", 1);

    delegate.unbind(id2);
    delegate.cast(0, 9.2f);

    println("{:*^60}", 2);
    Delegate_SingleCast<void, int, float> single_delegate;

    static_assert(std::is_invocable_r_v<void, decltype(&A::func), A*, int, float>);
    single_delegate.bind(lambda);
    delegate.cast(11, 2.1f);
    single_delegate.bind(func);
    delegate.cast(22, 5.1f);
}
