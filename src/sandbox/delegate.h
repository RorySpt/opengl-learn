#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <tuple>
template<typename  ...Args>
struct Delegate_MultiCast;
template<typename Ret, typename  ...Args>
struct Delegate_SingleCast;

template<typename  ...Args>
struct Delegate_Object
{
    using cb_func_type = void(Args...);
    using inner_func_type = std::function<cb_func_type>;

    /**
	 * Execute pending callback functions.
	 *
	 * This function is intended to be called in the user's designated thread,
	 * where the callback functions reside. It performs the following steps:
	 *
	 * 1. Locks the internal mutex to ensure thread safety.
	 * 2. Copies the pending callback functions to a local variable for processing.
	 * 3. Unlocks the mutex to allow other threads to continue adding callbacks.
	 * 4. Invokes each callback function in the order they were added.
	 *
	 * Note: It is the user's responsibility to ensure that this function is
	 * called in the correct thread context. Calling it from an incorrect thread
	 * may lead to unexpected behavior or errors.
	 */
    void tick() // thread safe; call this in the user thread, the thread where cb resides
    {
        mutex.lock();
        decltype(_calls) calls;
        std::swap(calls,_calls);
        mutex.unlock();

        for(auto& call: calls)
        {
            invoke_helper(call);
        }
        
    }
    void feed_for_call(Args... args) // thread safe;
    {
        std::lock_guard guard(mutex);
        _calls.emplace_back(std::forward<Args>(args)...);
    }
    //void feed_for_call(std::tuple<Args...>&& tuple)
    //{
    //    calls.push_back(tuple);
    //}
    void feed_for_call(const std::tuple<Args...>& tuple) // thread safe;
    {
        std::lock_guard guard(mutex);
        _calls.push_back(tuple);
    }

   
    void invoke_helper(std::tuple<Args...>& args) // thread unsafe;
    {
        invoke_helper(args, std::index_sequence_for<Args...>{});
    }
    template<typename T, T... ints>
    void invoke_helper(std::tuple<Args...>& args, std::integer_sequence<T, ints...>) // thread unsafe;
    {
        _cb(std::get<ints>(args) ...);
    }
    inner_func_type _cb;
    std::vector<std::tuple<Args...>> _calls{};
    std::mutex mutex;
};

//template<typename T , typename ...Args>
//concept DelegateCastConcept = std::is_same_v<T, Delegate_MultiCast<Args...>> || std::is_same_v<T, Delegate_SingleCast<Args...>>;

struct Delegate_Handle_Base  // NOLINT(cppcoreguidelines-special-member-functions)
{
	void* owner = nullptr;
    void* id = nullptr;
};

template<typename  ...Args>
struct Delegate_MultiCast_Handle: Delegate_Handle_Base {
    std::shared_ptr<Delegate_Object<Args...>> object{};

    void unbind()
    {
        if(owner == nullptr || id == nullptr) return;
        static_cast<Delegate_MultiCast<Args...>*>(owner)->unbind(*this);
    }
    void tick()
    {
        if (object)object->tick();
    }
};
template<typename  ...Args>
struct Delegate_SingleCast_Handle : Delegate_Handle_Base {

    void unbind()
    {
        if (owner == nullptr || id == nullptr) return;
        static_cast<Delegate_SingleCast<Args...>*>(owner)->unbind(*this);
    }
};




namespace ExecutionPolicy {
    struct SyncPolicy{};
    struct AsyncPolicy{};

    constexpr SyncPolicy Sync;
    constexpr AsyncPolicy Async;
};
template<typename T>
concept ExecutionPolicyConcept = std::is_same_v<T, ExecutionPolicy::SyncPolicy> || std::is_same_v<T, ExecutionPolicy::AsyncPolicy>;


template<typename  ...Args>
struct Delegate_MultiCast
{
    using type = Delegate_MultiCast;
    using cb_func_type = void(Args...);
    using inner_func_type = std::function<cb_func_type>;
    using handle = Delegate_MultiCast_Handle<Args...>;


    template<typename FuncType, ExecutionPolicyConcept EP = ExecutionPolicy::SyncPolicy>
		requires std::is_invocable_v<FuncType, Args...>
    handle bind(FuncType func, EP ep = ExecutionPolicy::Sync) noexcept
    {
        if constexpr (std::is_same_v<EP, ExecutionPolicy::SyncPolicy>)
        {
            _cbs.emplace_back(std::make_unique<inner_func_type>(func));
            return { this, _cbs.back().get() };  // NOLINT(clang-diagnostic-missing-braces)
        }else 
        {
            auto delegate_object = std::make_shared<Delegate_Object<Args...>>(func);
            _cbs.emplace_back(std::make_unique<inner_func_type>([delegate_object](Args... args)
                {
                    delegate_object->feed_for_call(std::forward<Args>(args)...);
                }));
            return { this, _cbs.back().get() , delegate_object};
        }
        
    }
    template<typename ClassType, typename FuncType, ExecutionPolicyConcept EP = ExecutionPolicy::SyncPolicy>
        requires std::is_invocable_v<FuncType, ClassType*, Args...> && std::is_class_v<ClassType>
    handle bind(ClassType* object, FuncType func, EP ep = ExecutionPolicy::Sync) noexcept
    {
        if (object == nullptr)return {};
        if constexpr (std::is_same_v<EP, ExecutionPolicy::SyncPolicy>)
        {
            _cbs.emplace_back(std::make_unique<inner_func_type>([object, func](Args... args)
                {
                    //(object->*func)(std::forward<Args>(args)...);
                    std::invoke(func, object, std::forward<Args>(args)...);
                }));
            return { this, _cbs.back().get() };
        }else
        {
            auto delegate_object = std::make_shared<Delegate_Object<Args...>>(
                std::make_unique<inner_func_type>([object, func](Args... args)
                {
                    //(object->*func)(std::forward<Args>(args)...);
                    std::invoke(func, object, std::forward<Args>(args)...);
                }));
            _cbs.emplace_back([delegate_object](Args... args)
                {
                    delegate_object->feed_for_call(std::forward<Args>(args)...);
                });
            return { this, _cbs.back().get() , delegate_object };
        }
        
    }
    //int unbind(const Delegate_Handle& handle) noexcept
    //{
    //    if(handle)
	//		return unbind(handle->id);
    //    return 0;
    //}
    int unbind(handle handle) noexcept
    {
        if (handle.owner != this || handle.id == nullptr)
            return 0;
        auto id = handle.id;
        return static_cast<int>(
            std::erase_if(_cbs, [id](std::unique_ptr<inner_func_type>& ptr)
                {
                    return ptr.get() == id;
                }));
    }
    void cast(Args... args) noexcept
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
    using type = Delegate_SingleCast;
    using cb_func_type = Ret(Args...);
    using inner_func_type = std::function<cb_func_type>;
    using handle = Delegate_SingleCast_Handle<Args...>;


    template<typename FuncType> requires std::is_invocable_r_v<Ret, FuncType, Args...>
    handle bind(FuncType func) noexcept
    {
        _cb = std::make_unique<inner_func_type>(func);
        return { this, _cb.get() };
    }
    template<typename ClassType, typename FuncType>
        requires std::is_invocable_r_v<Ret, FuncType, ClassType*, Args...>
    handle bind(ClassType* object, FuncType func) noexcept
    {
        if (object == nullptr)return {};
        _cb = std::make_unique<inner_func_type>([object, func](Args... args)
            {
                //(object->*func)(std::forward<Args>(args)...);
                std::invoke(func, object, std::forward<Args>(args)...);
            });
        return { this, _cb.get() };
    }

    //int unbind(const Delegate_Handle& handle) noexcept
    //{
    //    if (handle)
    //        return unbind(handle->id);
    //    return 0;
    //}

    int unbind(handle handle) noexcept
    {
        //if (handle.owner != this) return 0;
        return handle.id == _cb.get() ? (_cb.reset(nullptr), 1) : 0;
    }

    Ret cast(Args... args) noexcept
    {
        return (*_cb)(std::forward<Args>(args)...);
    }

    std::unique_ptr<inner_func_type> _cb;

};

void delegate_unit_test();