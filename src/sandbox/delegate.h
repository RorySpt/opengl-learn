#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <tuple>

template <typename... Args>
struct Delegate_MultiCast;
template <typename Ret, typename... Args>
struct Delegate_SingleCast;

template <typename... Args>
struct Delegate_Object
{
	using cb_func_t = void(Args...);
	using inner_func_t = std::function<cb_func_t>;

	void tick() // thread-unsafe; Call it in the user thread, which is the thread where the cb is located
	{
		mutex.lock();
		decltype(_calls) calls;
		std::swap(calls, _calls);
		mutex.unlock();

		for (auto& call : calls)
		{
			invoke_helper(call);
		}
	}

	void feed_for_call(Args&&... args) // thread safe;
	{
		std::lock_guard guard(mutex);
		_calls.emplace_back(std::forward<Args>(args)...);
	}

	//void feed_for_call(std::tuple<Args...>&& tuple)
	//{
	//    calls.push_back(tuple);
	//}
	//void feed_for_call(const std::tuple<Args...>& tuple) // thread safe;
	//{
	//    std::lock_guard guard(mutex);
	//    _calls.push_back(tuple);
	//}


	void invoke_helper(std::tuple<Args...>& args) // thread unsafe;
	{
		invoke_helper(args, std::index_sequence_for<Args...>{});
	}

	template <typename T, T... ints>
	void invoke_helper(std::tuple<Args...>& args, std::integer_sequence<T, ints...>) // thread unsafe;
	{
		_cb(std::get<ints>(args)...);
	}

	inner_func_t _cb;
	std::vector<std::tuple<Args...>> _calls{};
	std::mutex mutex{};
};


struct Delegate_Handle_Base // NOLINT(cppcoreguidelines-special-member-functions)
{
	virtual ~Delegate_Handle_Base() = default;  // NOLINT(clang-diagnostic-deprecated-copy-with-dtor)
	Delegate_Handle_Base(void* owner_ = nullptr, void* id_ = nullptr):owner(owner_),id(id){}
	void* owner = nullptr;
	void* id = nullptr;

	virtual void tick() {}
	virtual void unbind() {}
};

template <typename... Args>
struct Delegate_MultiCast_Handle : Delegate_Handle_Base
{
	std::shared_ptr<Delegate_Object<Args...>> object{};
	std::shared_ptr<std::function<void(Delegate_MultiCast_Handle)>> unbind_callback{};
	//CLASS_DEFAULT_COPY_AND_MOVE(Delegate_MultiCast_Handle)
	Delegate_MultiCast_Handle(void* owner_ = nullptr, void* id_ = nullptr
		, std::shared_ptr<Delegate_Object<Args...>> object_ = {}  // NOLINT(modernize-pass-by-value)
		, std::shared_ptr<std::function<void(Delegate_MultiCast_Handle)>> unbind_callback_ = {}  // NOLINT(modernize-pass-by-value)
		) :Delegate_Handle_Base(owner_, id_), object(object_), unbind_callback(unbind_callback_){}
	void unbind() override
	{
		if (owner == nullptr || id == nullptr) return;
		static_cast<Delegate_MultiCast<Args...>*>(owner)->unbind(*this);
		if (unbind_callback)(*unbind_callback)(*this);
	}

	void tick() override
	{
		if (object)object->tick();
	}
};


template <typename Ret, typename... Args>
struct Delegate_SingleCast_Handle : Delegate_Handle_Base
{
	Delegate_SingleCast_Handle(void* owner_ = nullptr, void* id_ = nullptr) :Delegate_Handle_Base(owner_, id_){}
	void unbind() override
	{
		if (owner == nullptr || id == nullptr) return;
		static_cast<Delegate_SingleCast<Ret, Args...>*>(owner)->unbind(*this);
	}
};

template <typename T>
struct delegate_handle_handler
{
	template <typename H>
	void add_handle(const T& obj, H& handle) = delete;

	template <typename H>
	void remove_handle(const T& obj, H& handle) = delete;
};


template <typename T>
concept Delegate_Bindable_Objcet = requires(T&& object, delegate_handle_handler<T> handler,
	Delegate_Handle_Base handle)
{
	handler.add_handle(std::forward<T>(object), handle);
	handler.remove_handle(std::forward<T>(object), handle);
};


namespace ExecutionPolicy
{
	struct SyncPolicy
	{
	};

	struct AsyncPolicy
	{
	};

	constexpr SyncPolicy Sync;
	constexpr AsyncPolicy Async;
};

template <typename T>
concept ExecutionPolicyConcept = std::is_same_v<T, ExecutionPolicy::SyncPolicy> || std::is_same_v<
	T, ExecutionPolicy::AsyncPolicy>;


template <typename... Args>
struct Delegate_MultiCast
{
	using self_t = Delegate_MultiCast;
	using cb_t = void(Args...);
	using func_t = std::function<cb_t>;
	using handle_t = Delegate_MultiCast_Handle<Args...>;


	template <typename Callable, ExecutionPolicyConcept EP = ExecutionPolicy::SyncPolicy>
		requires std::is_invocable_v<Callable, Args...>
	handle_t bind(Callable func, EP ep = ExecutionPolicy::Sync) noexcept
	{
		if constexpr (std::is_same_v<EP, ExecutionPolicy::SyncPolicy>)
		{
			_cbs.emplace_back(std::make_unique<func_t>(func));
			return {this, _cbs.back().get()}; // NOLINT(clang-diagnostic-missing-braces)
		}
		else
		{
			auto delegate_object = std::make_shared<Delegate_Object<Args...>>(func);
			_cbs.emplace_back(std::make_unique<func_t>([delegate_object](Args... args)
			{
				delegate_object->feed_for_call(std::forward<Args>(args)...);
			}));
			return {this, _cbs.back().get(), delegate_object};
		}
	}

	template <typename ClassType, typename Callable, ExecutionPolicyConcept EP = ExecutionPolicy::SyncPolicy>
		requires (std::is_member_function_pointer_v<Callable> && std::is_invocable_v<Callable, ClassType*, Args...>
		|| !std::is_member_function_pointer_v<Callable> && std::is_invocable_v<Callable, Args...>) && std::is_class_v<ClassType>
	handle_t bind(ClassType* object, Callable func, EP ep = ExecutionPolicy::Sync) noexcept
	{
		if (object == nullptr)return {};
		if constexpr (std::is_same_v<EP, ExecutionPolicy::SyncPolicy>)
		{
			_cbs.emplace_back(std::make_unique<func_t>([object, func](Args... args)
			{
				//(object->*func)(std::forward<Args>(args)...);
				if constexpr (std::is_member_function_pointer_v<Callable>)
					std::invoke(func, object, std::forward<Args>(args)...);
				else
					std::invoke(func, std::forward<Args>(args)...);
			}));
			return {this, _cbs.back().get()};
		}
		else
		{
			auto delegate_object = std::make_shared<Delegate_Object<Args...>>(
				[object, func](Args... args)
				{
					//(object->*func)(std::forward<Args>(args)...);
					if constexpr (std::is_member_function_pointer_v<Callable>)
						std::invoke(func, object, std::forward<Args>(args)...);
					else
						std::invoke(func, std::forward<Args>(args)...);
				});
			_cbs.emplace_back(std::make_unique<func_t>([delegate_object](Args&&... args)
			{
				delegate_object->feed_for_call(std::forward<Args>(args)...);
			}));

			if constexpr (Delegate_Bindable_Objcet<std::remove_cvref_t<ClassType>>)
			{
				delegate_handle_handler<std::remove_cvref_t<ClassType>> handler;
				handle_t handle{
					this, _cbs.back().get(), delegate_object
					, std::make_shared<std::function<void(handle_t)>>([=](handle_t h) { handler.remove_handle(*object, h); })
				};
				handler.add_handle(*object, handle);
				return handle;
			}
			else
			{
				return {this, _cbs.back().get(), delegate_object};
			}
		}
	}

	//int unbind(const Delegate_Handle& handle) noexcept
	//{
	//    if(handle)
	//		return unbind(handle->id);
	//    return 0;
	//}
	int unbind(handle_t handle) noexcept
	{
		if (handle.owner != this || handle.id == nullptr)
			return 0;
		auto id = handle.id;
		return static_cast<int>(
			std::erase_if(_cbs, [id](std::unique_ptr<func_t>& ptr)
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

	std::vector<std::unique_ptr<func_t>> _cbs;
};

template <typename Ret, typename... Args>
struct Delegate_SingleCast
{
	using self_t = Delegate_SingleCast;
	using cb_t = Ret(Args...);
	using function_t = std::function<cb_t>;
	using handle_t = Delegate_SingleCast_Handle<Ret, Args...>;


	template <typename Callable> requires std::is_invocable_r_v<Ret, Callable, Args...>
	handle_t bind(Callable func) noexcept
	{
		_cb = std::make_unique<function_t>(func);
		return {this, _cb.get()};
	}

	template <typename ClassType, typename Callable>
		requires std::is_invocable_r_v<Ret, Callable, ClassType*, Args...>
	handle_t bind(ClassType* object, Callable func) noexcept
	{
		if (object == nullptr)return {};
		_cb = std::make_unique<function_t>([object, func](Args... args)
		{
			//(object->*func)(std::forward<Args>(args)...);
			std::invoke(func, object, std::forward<Args>(args)...);
		});
		return {this, _cb.get()};
	}

	//int unbind(const Delegate_Handle& handle) noexcept
	//{
	//    if (handle)
	//        return unbind(handle->id);
	//    return 0;
	//}

	int unbind(handle_t handle) noexcept
	{
		//if (handle.owner != this) return 0;
		return handle.id == _cb.get() ? (_cb.reset(nullptr), 1) : 0;
	}

	Ret cast(Args... args) noexcept
	{
		return (*_cb)(std::forward<Args>(args)...);
	}

	std::unique_ptr<function_t> _cb;
};

void delegate_unit_test();
