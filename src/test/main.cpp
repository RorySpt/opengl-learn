#include <format>
#include <iostream>
#include <functional>

template <class... Types>
void println(const std::string_view fmt, Types&&... args) { std::cout << std::vformat(fmt, std::make_format_args(args...)) << "\n"; }
template <class... Types>
void print(const std::string_view fmt, Types&&... args) { std::cout << std::vformat(fmt, std::make_format_args(args...)); }


template<typename  ...Args>
struct Delegate_MultiCast
{
    using cb_func_type = void(Args...);
    using inner_func_type = std::function<cb_func_type>;
    using handle = void*;

    template<typename FuncType> requires std::is_invocable_v<FuncType, Args...>
    handle bind(FuncType func)
    {
    	_cbs.emplace_back(std::make_unique<inner_func_type>(func));
        return _cbs.back().get();
    }
    template<typename ClassType, typename FuncType>
		requires std::is_invocable_v<FuncType, ClassType*, Args...>
    handle bind(ClassType* object, FuncType func)
    {
        if (object == nullptr)return nullptr;
        _cbs.emplace_back(std::make_unique<inner_func_type>([object, func](Args... args)
	        {
                //(object->*func)(std::forward<Args>(args)...);
                std::invoke(func, object, std::forward<Args>(args)...);
	        }));
        return _cbs.back().get();
    }
    int unbind(handle handle)
    {
        return static_cast<int>(
            std::erase_if(_cbs, [handle](std::unique_ptr<inner_func_type>& ptr)
            {
                return ptr.get() == handle;
            }));
    }
    void cast(Args... args)
    {
	    for(auto& cb:_cbs)
	    {
            (*cb)(std::forward<Args>(args)...);
	    }
    }
    std::vector<std::unique_ptr<inner_func_type>> _cbs;
};
template<typename Ret, typename  ...Args>
struct Delegate_SingleCast
{
    using cb_func_type = Ret(Args...);
    using inner_func_type = std::function<cb_func_type>;
    using handle = void*;

    template<typename FuncType> requires std::is_invocable_r_v<Ret, FuncType, Args...>
    handle bind(FuncType func)
    {
        _cb = std::make_unique<inner_func_type>(func);
        return _cb.get();
    }
    template<typename ClassType, typename FuncType>
        requires std::is_invocable_r_v<Ret, FuncType, ClassType*, Args...>
    handle bind(ClassType* object, FuncType func)
    {
        if (object == nullptr)return nullptr;
        _cb = std::make_unique<inner_func_type>([object, func](Args... args)
            {
                //(object->*func)(std::forward<Args>(args)...);
                std::invoke(func, object, std::forward<Args>(args)...);
            });
        return _cb.get();
    }

    int unbind(handle handle)
    {
        return handle == _cb.get() ? (_cb.reset(nullptr), 1) : 0;
    }

    Ret cast(Args... args)
    {
        return (*_cb)(std::forward<Args>(args)...);
    }

    std::unique_ptr<inner_func_type> _cb;

};

void func(int a, float b)
{
    println("{}: int({}), float({})",__FUNCTION__, a, b);
}
struct A
{
    void func(int a, float b)
    {
        println("{}: int({}), float({})", __FUNCTION__, a, b);
    }
};
int main()
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

    println("{:*>60}"," ");

    delegate.unbind(id2);
    delegate.cast(0, 9.2f);

    println("{:*>60}", " ");
    Delegate_SingleCast<void, int, float> single_delegate;

    static_assert(std::is_invocable_r_v<void, decltype(&A::func), A*, int, float>);
    single_delegate.bind(lambda);
    delegate.cast(11, 2.1f);
    single_delegate.bind(func);
    delegate.cast(22, 5.1f);
}	

