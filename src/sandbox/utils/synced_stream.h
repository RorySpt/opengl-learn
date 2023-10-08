#pragma once
#include <cstdio>
#include <iostream>
#include <ostream>
#include <print>
#include <type_traits>

namespace sync
{
	namespace details
	{
		enum class Add_newline : bool { Nope, Yes };
		inline void print_impl(const Add_newline Add_nl, FILE* const Stream, const std::string_view s)
		{
			if (!s.empty())
			{
				fwrite(s.data(), sizeof std::string_view::value_type, s.size(), Stream);
			}
			if (Add_nl == Add_newline::Yes)
			{
				fwrite("\n", sizeof std::string_view::value_type, 1, Stream);
			}
		}
		inline void print_impl(const Add_newline Add_nl, std::ostream& Stream, const std::string_view s)
		{
			if (!s.empty())
			{
				Stream.write(s.data(), static_cast<std::streamsize>(s.size()));
			}
			if (Add_nl == Add_newline::Yes)
			{
				Stream.put('\n');
			}
		}
	}
	template<typename T = FILE> requires std::is_same_v<T, FILE> || std::is_same_v<T, std::ostream>
	struct synced_stream
	{
		using storage_t = std::conditional_t<std::is_same_v<T, FILE>,
			std::add_pointer_t<FILE>, std::add_lvalue_reference_t<std::ostream>>;

		synced_stream(storage_t file) : out(file) {}

		template <class... Types>
		void print(const std::format_string<Types...> fmt, Types&&... args) {
			std::lock_guard guard(mutex);
			std::print(out, fmt, std::forward<Types>(args)...);
		}
		template <class... Types>
		void println(const std::format_string<Types...> fmt, Types&&... args) {
			std::lock_guard guard(mutex);
			std::println(out, fmt, std::forward<Types>(args)...);
		}


		// Unformatted string printing

		void print(std::string_view s)
		{
			std::lock_guard guard(mutex);
			details::print_impl(details::Add_newline::Nope, out, s);
		}
		void println(std::string_view s)
		{
			std::lock_guard guard(mutex);
			details::print_impl(details::Add_newline::Yes, out, s);
		}
	private:
		storage_t out;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
		std::mutex mutex;
	};

	inline synced_stream sync_out(stdout);
	inline synced_stream<std::ostream> sync_cout(std::cout);

	template <class... Types>
	void print(const std::format_string<Types...> fmt, Types&&... args) {
		sync_out.print(fmt, std::forward<Types>(args)...);
	}
	template <class... Types>
	void println(const std::format_string<Types...> fmt, Types&&... args) {
		sync_out.println(fmt, std::forward<Types>(args)...);
	}

	inline void print(std::string_view s)
	{
		sync_out.print(s);
	}

	inline void println(std::string_view s)
	{
		sync_out.println(s);
	}
}
