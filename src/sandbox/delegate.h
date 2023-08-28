#pragma once
#include <functional>
#include <memory>

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
        for (auto& cb : _cbs)
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

void delegate_unit_test();