module;
#include <functional>
#include <mutex>
#include <map>
#include <print>
export module signals;



export namespace utils
{
	template<typename ...Args>
	struct signal
	{
		using slot_t = std::function<void(Args...)>;
		using id_t = size_t;

		struct connection
		{
			id_t id;
		};

		template<typename Func> requires std::is_invocable_v<Func, Args...>
		auto connect(Func&& f) -> connection
		{
			using ret_t = std::invoke_result_t<Func, Args...>;
			std::lock_guard guard(mutex);
			auto id = id_count++;
			slots.emplace(id, [=](Args&& ...args)
				{
					if constexpr (sizeof...(Args) > 0)
						f(std::forward<Args>(args)...);
					else
						f();
				});
			return connection{ id };
		}

		std::size_t disconnect(connection connection)
		{
			std::lock_guard guard(mutex);
			return slots.erase(connection.id);
		}
		void emit(Args&& ...args)
		{
			std::lock_guard guard(mutex);
			for (auto& [id, slot] : slots)
				if constexpr (sizeof...(Args) > 0)
					slot(std::forward<Args>(args)...);
				else
					slot();
		}
		void operator()(Args&& ...args)
		{
			emit(std::forward<Args>(args)...);
		}
		
		std::mutex mutex;
		id_t id_count = 0;
		std::map<id_t, slot_t> slots;
	};

	template<typename Ret, typename ...Args>
		requires !std::is_same_v<Ret, void>
	struct signal_r
	{
		using slot_t = std::function<Ret(Args...)>;
		using id_t = size_t;

		struct connection
		{
			id_t id;
		};

		template<typename Func> requires std::is_invocable_r_v<Ret, Func, Args...>
		auto connect(Func&& f) -> connection
		{
			//using ret_t = std::invoke_result_t<Ret, Func, Args...>;
			std::lock_guard guard(mutex);
			auto id = id_count++;
			slots.emplace(id , [=](Args&& ...args)
				{
					if constexpr (sizeof...(Args) > 0)
						return f(std::forward<Args>(args)...);
					else
						return f();
				});
			return { id };
		}

		std::size_t disconnect(connection connection)
		{
			std::lock_guard guard(mutex);
			return slots.erase(connection.id);
		}
		auto emit(Args&& ...args) -> std::vector<std::pair<connection, Ret>>
		{
			std::vector<std::pair<connection, Ret>> r_vec;
			std::lock_guard guard(mutex);
			for (auto& [id, slot] : slots)
				r_vec.emplace_back(id, slot(std::forward<Args>(args)...));
			return r_vec;
		}
		auto operator()(Args&& ...args) -> std::vector<std::pair<connection, Ret>>
		{
			return emit(std::forward<Args>(args)...);
		}


		std::mutex mutex;
		id_t id_count = 0;
		std::map<id_t, slot_t> slots;
	};
}


int foo(std::string_view s)
{
	static int i = 0;
	std::println(R"({}, s: "{}")", __FUNCTION__, s);
	return i++;
}


export void signals_test()
{
	utils::signal<> signal;
	using slot_t = utils::signal<>::slot_t;
	signal.connect(std::bind_front(&foo, "signal"));
	signal.connect(std::bind_front(&foo, "signal1"));
	signal.connect(std::bind_front(&foo, "signal2"));

	signal.emit();

	utils::signal<std::string_view> signal_s;

	signal_s.connect(&foo);
	signal_s.connect(&foo);
	signal_s.connect(&foo);

	signal_s.emit("signal_s");

	utils::signal_r<int> signal_r;
	signal_r.connect(std::bind_front(&foo, "signal"));
	signal_r.connect(std::bind_front(&foo, "signal1"));
	signal_r.connect(std::bind_front(&foo, "signal2"));

	auto ret = signal_r.emit();
	for(auto& [c, i]: ret)
	{
		std::print("{{id: {}, count: {}}}", c.id, i);
	}
	std::println("");


}