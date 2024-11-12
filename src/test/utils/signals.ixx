module;
#include <functional>
#include <map>
#include <mutex>
#include <print>
#include <ranges>
export module signals;



export namespace utils
{
	template<typename ...Args>
	struct signal
	{
		using slot_t = std::function<void(Args...)>;
		using id_t = size_t;

		static inline constexpr bool is_null_param_v = sizeof...(Args) == 0;

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
		template <typename ...InArgs> requires std::is_invocable_v<slot_t, InArgs...>
		void emit(InArgs&& ...args)
		{
			std::lock_guard guard(mutex);
			if (slots.empty()) return;
			auto values = slots | std::views::values;
			if constexpr (is_null_param_v)
			{
				for (auto& slot : values) slot();
			}
			else
			{
				for (auto& slot : values | std::views::take(slots.size() - 1))
					slot(args...);
				values.back()(std::forward<InArgs>(args)...);
			}
			
		}
		template <typename ...InArgs> requires std::is_invocable_v<slot_t, InArgs...>
		void operator()(InArgs&& ...args)
		{
			emit(std::forward<InArgs>(args)...);
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

		static inline constexpr bool is_null_param_v = sizeof...(Args) == 0;

		struct connection
		{
			id_t id;
		};

		template<typename Func> requires std::is_invocable_r_v<Ret, Func, Args...>
		auto connect(Func&& f) -> connection
		{
			//using ret_t = std::invoke_result_t<Ret, Func, Args...>;
			std::lock_guard guard(mutex);
			auto id = _idCount++;
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
		template <typename ...InArgs> requires std::is_invocable_v<slot_t, InArgs...>
		auto emit(InArgs&& ...args) -> std::vector<std::pair<connection, Ret>>
		{
			std::lock_guard guard(mutex);
			if (slots.empty()) return{};
			std::vector<std::pair<connection, Ret>> r_vec;
			auto values = slots | std::views::all;
			if constexpr (is_null_param_v)
			{
				for (auto& [id, slot] : values) r_vec.emplace_back(id, slot()) ;
			}
			else
			{
				for (auto& [id, slot] : values | std::views::take(slots.size() - 1))
					r_vec.emplace_back(id, slot(args...));
				r_vec.emplace_back(values.back().first, values.back().second(std::forward<InArgs>(args)...));
			}
			return r_vec;
		}
		template <typename ...InArgs> requires std::is_invocable_v<slot_t, InArgs...>
		auto operator()(InArgs&& ...args) -> std::vector<std::pair<connection, Ret>>
		{
			return emit(std::forward<InArgs>(args)...);
		}

	private:
		std::mutex mutex;
		id_t _idCount = 0;
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