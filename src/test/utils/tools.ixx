module;
#include <chrono>
#include <cstdio>
#include <format>
#include <mutex>
#include <print>
#include <string_view>
export module tools;


namespace std // Õÿ’πprint
{

	//enum class _Add_newline : bool { _Nope, _Yes };
	void _print_impl(const _Add_newline _Add_nl, FILE* const _Stream, const string_view s)
	{
		if(!s.empty())
		{
			fwrite(s.data(), sizeof string_view::value_type, s.size(), _Stream);
		}
		if(_Add_nl == _Add_newline::_Yes)
		{
			fwrite("\n", sizeof string_view::value_type, 1, _Stream);
		}
	}


	_EXPORT_STD template <int = 0>
		void print(FILE* const _Stream, const string_view s) {
		_STD _print_impl(_Add_newline::_Nope, _Stream, s);
	}
	_EXPORT_STD template <int = 0>
		void print(const string_view s) {
		_STD print(stdout, s);
	}

	_EXPORT_STD template <int = 0>
		void println(FILE* const _Stream, const string_view s) {
		_STD _print_impl(_Add_newline::_Yes, _Stream, s);
	}
	_EXPORT_STD template <int = 0>
		void println(const string_view s) {
		_STD println(stdout, s);
	}
}


namespace utils
{
	export class Log
	{
	public:
		inline static std::mutex mutex;
		template <class... Types>
		static void print(const std::format_string<Types...> fmt, Types&&... args) {
			std::lock_guard guard(mutex);
			std::print("[LOG][{:L%F %H:%M}]: ", std::chrono::zoned_time{
				std::chrono::current_zone(),
				std::chrono::system_clock::now()
				});
			std::print(stdout, fmt, std::forward<Types>(args)...);
		}
		template <class T>
		static void print(T&& arg) {
			std::lock_guard guard(mutex);
			std::print("[LOG][{:L%F %H:%M}]: ", std::chrono::zoned_time{
				std::chrono::current_zone(),
				std::chrono::system_clock::now()
				});

			if constexpr (std::convertible_to<const T&, std::string_view>)
			{
				std::print(arg);
			}
			else
			{
				std::print("{}", arg);
			}
		}

		template <class... Types>
		static void println(const std::format_string<Types...> fmt, Types&&... args) {
			std::lock_guard guard(mutex);
			std::print("[LOG][{:L%F %H:%M}]: ", std::chrono::zoned_time{
				std::chrono::current_zone(),
				std::chrono::system_clock::now()
				});
			std::println(fmt, std::forward<Types>(args)...);
		}

		template <class T>
		static void println(T&& arg) {
			std::lock_guard guard(mutex);
			std::print("[LOG][{:L%F %H:%M}]: ", std::chrono::zoned_time{
				std::chrono::current_zone(),
				std::chrono::system_clock::now()
				});

			if constexpr (std::convertible_to<const T&, std::string_view>)
			{
				std::println(arg);
			}
			else
			{
				std::println("{}", arg);
			}
		}
	};
}

