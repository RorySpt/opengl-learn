#pragma once
#include <type_traits>

//from Qt

template<typename Enum> 
class flags	
{
public:
	class flag
	{
		int i;
	public:
		constexpr flag(const int value)noexcept :i(value){}
		constexpr operator int() const noexcept { return i; }
		
	};

	typedef std::conditional_t<
		std::is_unsigned_v<std::underlying_type_t<Enum>>,
		unsigned int,
		signed int
	> Int;
	static_assert(sizeof(Enum) <= sizeof(int) && std::is_enum_v<Enum>,"Enum is only usable on enumeration types.");
	constexpr flags(Enum flags) noexcept : i(static_cast<Int>(flags)) {}
	constexpr inline flags(flag flag) noexcept : i(flag) {}
	constexpr flags(std::initializer_list<Enum> flags) noexcept{
		for (auto f : flags){ *this |= f; }
	}

	constexpr inline flags& operator&=(const int mask) noexcept { i &= mask; return *this; }
	constexpr inline flags& operator&=(const unsigned mask) noexcept { i &= mask; return *this; }
	constexpr inline flags& operator&=(Enum mask) noexcept { i &= static_cast<Int>(mask); return *this; }
	constexpr inline flags& operator|=(flags other) noexcept { i |= other.i; return *this; }
	constexpr inline flags& operator|=(Enum other) noexcept { i |= static_cast<Int>(other); return *this; }
	constexpr inline flags& operator^=(flags other) noexcept { i ^= other.i; return *this; }
	constexpr inline flags& operator^=(Enum other) noexcept { i ^= static_cast<Int>(other); return *this; }

	constexpr inline operator Int() const noexcept { return i; }



	constexpr inline flags operator|(flags other) const noexcept { return flags(flag(i | other.i)); }
	constexpr inline flags operator|(Enum other) const noexcept { return flags(flag(i | Int(other))); }
	constexpr inline flags operator^(flags other) const noexcept { return flags(flag(i ^ other.i)); }
	constexpr inline flags operator^(Enum other) const noexcept { return flags(flag(i ^ Int(other))); }
	constexpr inline flags operator&(int mask) const noexcept { return flags(flag(i & mask)); }
	constexpr inline flags operator&(unsigned mask) const noexcept { return flags(flag(i & mask)); }
	constexpr inline flags operator&(Enum other) const noexcept { return flags(flag(i & Int(other))); }
	constexpr inline flags operator~() const noexcept { return flags(flag(~i)); }

	constexpr inline bool operator!() const noexcept { return !i; }

	constexpr inline bool testFlag(Enum flag) const noexcept { return (i & Int(flag)) == Int(flag) && (Int(flag) != 0 || i == Int(flag)); }
	constexpr inline flags& setFlag(Enum flag, bool on = true) noexcept
	{
		return on ? (*this |= flag) : (*this &= ~Int(flag));
	}

private:
	constexpr static inline Int initializer_list_helper(typename std::initializer_list<Enum>::const_iterator it,
		typename std::initializer_list<Enum>::const_iterator end)
		noexcept
	{
		return (it == end ? static_cast<Int>(0) : (static_cast<Int>(*it) | initializer_list_helper(it + 1, end)));
	}

	Int i = 0;
};
#define Q_DECLARE_FLAGS(Flags, Enum)\
typedef flags<Enum> (Flags);


template<typename EnumType, typename CallBack>
	requires std::is_invocable_v<CallBack, EnumType>
auto DecomposeFlags(flags<EnumType> mods, CallBack cb)
{
	for (int i = 0, max = sizeof(int) * 8;
		i < max; ++i)
	{
		if (mods & (1u << i))
		{
			cb(static_cast<EnumType>(1u << i));
		}
	}
}
