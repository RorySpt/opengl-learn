//import std;

#include <algorithm>
#include <cctype>
#include <print>
#include <string_view>
#include <ranges>




constexpr std::string_view trim(const std::string_view sv)
{
	constexpr auto is_space = [](const int c) -> int {return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; };
	const auto first = std::ranges::find_if_not(sv, is_space);
	const auto last = std::ranges::find_if_not(sv | std::views::reverse, is_space).base() - 1;
	return last >= first ? sv.substr(first - sv.begin(), last - first + 1) : std::string_view{};
}
constexpr std::string_view trim1(const std::string_view sv)
{
	const auto first = sv.find_first_not_of(" \f\n\r\t\v");
	const auto last = sv.find_last_not_of(" \f\n\r\t\v");
	return last >= first ? sv.substr(first, last - first + 1) : std::string_view{};
}

[[nodiscard]]
constexpr bool is_space(char ch) noexcept
{
	switch (ch)
	{
	case ' ':
	case '\t':
	case '\n':
	case '\v':
	case '\r':
	case '\f':
		return true;
	}
	return false;
};

[[nodiscard]]
constexpr std::string_view trim_left(std::string_view const in) noexcept
{
	auto ranges = in | std::views::drop_while(is_space);
	return {ranges.begin(), ranges.end()};
}









//constexpr std::string_view trim2(const std::string_view sv)
//{
//	constexpr auto is_space = [](const int c) -> int {return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; };
//	auto ranges = sv | std::views::drop_while(is_space) /*| std::views::take_while([](const int c) {return !is_space(c); })*/;
//	return ranges;
//}
consteval std::string_view trim3(const std::string_view sv)
{
	constexpr auto is_space = [](const int c) -> int {return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; };
	const auto first = std::find_if_not(sv.cbegin(), sv.cend(), is_space);
	const auto last = std::find_if_not(sv.crbegin(), sv.crend(), is_space).base() - 1;
	return last >= first ? sv.substr(first - sv.begin(), last - first + 1) : std::string_view{};
}

void trim_unit_test()
{
	// 字面类型字符串
	{
		constexpr std::string_view sv = trim("   123   ");
		std::println("|{}|", sv); // 打印 |123|
	}
	// 常量字符串
	{
		const std::string s = "   123   ";
		const std::string_view sv = trim(s);
		std::println("|{}|", sv); // 打印 |123|
	}
	// 右值字符串
	{
		const std::string sv{ trim("   123   ") };
		std::println("|{}|", sv); // 打印 |123|
	}
}


consteval int f(int a, int b) {
	int c;
	do {
		c = a ^ b;
		b = a & b << 1;
		a = c;
	} while (b != 0);
	return c;
}



int main()
{
	constexpr int r = 47 & -42;


	trim_unit_test();

	constexpr auto is_space = [](const int c) -> int {return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; };

	constexpr std::string_view sv = "123456789";


	constexpr auto rrbegin = std::make_reverse_iterator(std::make_reverse_iterator(sv.begin()));
	constexpr auto rrend = std::make_reverse_iterator(std::make_reverse_iterator(sv.begin()));

	constexpr auto c = *rrbegin;
	
	for(auto it = rrbegin.base().base(); it != sv.end(); ++it)
	{
		std::print("{}", *it);
	}

}