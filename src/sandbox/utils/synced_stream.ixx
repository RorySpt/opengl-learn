module;
#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include <print>
#include <type_traits>
export module synced_stream;


namespace sync
{
	template<typename T = FILE> requires std::is_same_v<T, FILE> || std::is_same_v<T, std::ostream>
	struct synced_stream
	{
		using storage_t = std::conditional_t<std::is_same_v<T, FILE>,
			std::add_pointer_t<FILE>, std::add_lvalue_reference_t<std::ostream>>;

		synced_stream(storage_t file) : out(file) {}

		template <class... _Types>
		void print(const std::format_string<_Types...> _Fmt, _Types&&... _Args) {
			std::lock_guard guard(mutex);
			std::print(out, _Fmt, std::forward<_Types>(_Args)...);
		}
		template <class... _Types>
		void println(const std::format_string<_Types...> _Fmt, _Types&&... _Args) {
			std::lock_guard guard(mutex);
			std::println(out, _Fmt, std::forward<_Types>(_Args)...);
		}

	private:
		storage_t out;
		std::mutex mutex;
	};

	synced_stream sync_out(stdout);
	synced_stream<std::ostream> sync_cout(std::cout);

	export template <class... _Types>
		void print(const std::format_string<_Types...> _Fmt, _Types&&... _Args) {
		sync_out.print(_Fmt, std::forward<_Types>(_Args)...);
	}
	export template <class... _Types>
		void println(const std::format_string<_Types...> _Fmt, _Types&&... _Args) {
		sync_out.println(_Fmt, std::forward<_Types>(_Args)...);
	}
}
