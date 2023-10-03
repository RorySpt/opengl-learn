#pragma once
#include <cstdio>
#include <iostream>
#include <ostream>
#include <print>
#include <type_traits>

namespace sync
{
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

	private:
		storage_t out;
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
}
