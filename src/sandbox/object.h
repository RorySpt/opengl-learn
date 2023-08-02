#pragma once
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <string>

// 用于保证名字唯一
class DisplayNameGenerator
{
public:
	std::string operator()(std::string_view name)
	{
		if (!_names.contains(name.data()))
		{
			_names[name.data()] = 0;
			return std::string{name};
		}
		auto& index = _names[name.data()];
		index += 1;
		return std::format("{}_{}", name, index);
	}

	std::map<std::string, int> _names;
};

inline DisplayNameGenerator g_display_name_generator;


template<typename T>
class meta
{
public:
	virtual ~meta() = default;
	using Type = T;

	virtual std::string_view name() { return _name; }
	virtual std::string_view display_name()
	{
		return _display_name;
	}
	virtual std::string_view type_name()
	{
		const std::string_view name(typeid(Type).name());
		const auto pos = name.find_last_of(' ');
		if(pos != std::string_view::npos)
		{
			return name.substr(pos + 1);
		}
		return name;
	}

	std::string _name = type_name();
	std::string _display_name;
	DisplayNameGenerator* _self_display_name_generator = &g_display_name_generator;
private:
	int id = ++instance_count;
	inline static int instance_count = 0;
};



class object
{
public:
	std::string_view name() { return _name; }
	std::string_view display_name()
	{
		return _display_name;
	}
	std::string_view type_name()
	{
		const std::string_view name(typeid(*this).name());
		const auto pos = name.find_last_of(' ');
		if (pos != std::string_view::npos)
		{
			return name.substr(pos);
		}
		return name;
	}
	void set_self_display_name_generator(DisplayNameGenerator* generator)
	{
		_self_display_name_generator = generator;
	}
	void set_name(std::string_view name)
	{
			_name = name;																				 
			_display_name = (*_self_display_name_generator)(name); 
	}
private:
	std::string _name;
	std::string _display_name;
	DisplayNameGenerator* _self_display_name_generator = &g_display_name_generator;

	int id = ++instance_count;
	inline static int instance_count = 0;
};



#define ClassMetaDeclare(T)														  \
	public: std::string_view type_name()												  \
	{																			  \
		const std::string_view name(typeid(*this).name());				   \
		const auto pos = name.find_last_of(' ');						\
		if (pos != std::string_view::npos)										  \
		{																		  \
			return name.substr(pos + 1);										 \
		}																	   \
		return name;															\
	}
	//public:\
	//using Type = T;																					 \
	//constexpr static std::string_view TypeName = #T;



	/*virtual std::string_view name() { return T::_name; }													\
	virtual std::string_view display_name() \
	{\
		/*static std::string display_name;\
		display_name = id ? _name + "_" + std::to_string(id) : _name;\
		return _display_name;\
	}\
	virtual std::string_view type_name()																	 \
	{																								 \
		static const std::string name(typeid(Type).name());									   \
		const auto pos = name.find_last_of(' ');										   \
		if (pos != std::string_view::npos)															 \
		{																							 \
			return name.substr(pos);															\
		}																							 \
		return name;																				 \
	}																								 \
	void set_name(std::string_view name){																	\
		_name = name;																				 \
		_display_name = (*_self_display_name_generator)(name);\
	}\
	std::string _name = #T;																			\
	std::string _display_name = g_display_name_generator(#T);										\
	DisplayNameGenerator* _self_display_name_generator = &g_display_name_generator;				\
	private:																						\
		int id = instance_count++;																	\
		inline static int instance_count = 0;														*/

#define MetaClassDefine