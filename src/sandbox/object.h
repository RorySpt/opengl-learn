#pragma once
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <ranges>
#include <string>
#include "delegate.h"
// ���ڱ�֤����Ψһ
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
	virtual ~object() = default; // ����this��̬
	std::string_view name() { return _name; }
	std::string_view display_name()
	{
		return _display_name;
	}
	std::string_view type_name()
	{
		return get_type_name(this);
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
	template<typename T> requires std::is_base_of_v<object, std::remove_cvref_t<T>>
	static std::string_view get_type_name();
	static std::string_view get_type_name(object * obj);

private:
	std::string _name;
	std::string _display_name;
	DisplayNameGenerator* _self_display_name_generator = &g_display_name_generator;

	int id = ++instance_count;
	inline static int instance_count = 0;
};

template <typename T> requires std::is_base_of_v<object, std::remove_cvref_t<T>>
std::string_view object::get_type_name()
{
	static const std::string_view type_name(typeid(std::remove_cvref_t<T>).name());
	static const auto pos = type_name.find_last_of(' ');
	static const std::string_view name = pos != std::string_view::npos
		                                     ? type_name.substr(pos + 1)
		                                     : std::string_view{};
	return name;
}


// �����������ص�ͨѶ
extern std::map<const object*, std::vector<std::shared_ptr<Delegate_Handle_Base>>> object_handles;

template<typename T> requires std::is_base_of_v<object, std::remove_cvref_t<T>>
struct delegate_handle_handler<T>
{
	static void tick_handle() // ��������������԰��յ�����Ϣ�����������������߳�
	{
		for(const auto& handle : object_handles | std::views::values | std::views::join)
		{
			handle->tick();
		}
	}
	// ����ķ���Ϊ�ص�������֪ͨhandle�Ĵ������Ƴ�
	template<typename H> requires std::is_base_of_v<Delegate_Handle_Base, std::remove_cvref_t<H>>
	static void add_handle(const object& obj, H handle)
	{
		object_handles[&obj].emplace_back(std::dynamic_pointer_cast<Delegate_Handle_Base>(std::make_shared<H>(handle)) );
	}

	template<typename H> requires std::is_base_of_v<Delegate_Handle_Base, std::remove_cvref_t<H>>
	static void remove_handle(const object& obj, H handle)
	{
		std::erase_if(object_handles[&obj], [&](const auto& object_handle)
			{
				return object_handle->id == handle.id;
			});
	}


	
};


#define ClassMetaDeclare(T)														  \
	//public: std::string_view type_name()												  \
	//{																			  \
	//	const std::string_view name(typeid(*this).name());				   \
	//	const auto pos = name.find_last_of(' ');						\
	//	if (pos != std::string_view::npos)										  \
	//	{																		  \
	//		return name.substr(pos + 1);										 \
	//	}																	   \
	//	return name;															\
	//}
	//public:\
	//using Type = T;																					 \
	//constexpr static std::string_view TypeName = #T;



	//virtual std::string_view name() { return T::_name; }													\
	//virtual std::string_view display_name() \
	//{\
	//	/*static std::string display_name;\
	//	display_name = id ? _name + "_" + std::to_string(id) : _name;\
	//	return _display_name;\
	//}\
	//virtual std::string_view type_name()																	 \
	//{																								 \
	//	static const std::string name(typeid(Type).name());									   \
	//	const auto pos = name.find_last_of(' ');										   \
	//	if (pos != std::string_view::npos)															 \
	//	{																							 \
	//		return name.substr(pos);															\
	//	}																							 \
	//	return name;																				 \
	//}																								 \
	//void set_name(std::string_view name){																	\
	//	_name = name;																				 \
	//	_display_name = (*_self_display_name_generator)(name);\
	//}\
	//std::string _name = #T;																			\
	//std::string _display_name = g_display_name_generator(#T);										\
	//DisplayNameGenerator* _self_display_name_generator = &g_display_name_generator;				\
	//private:																						\
	//	int id = instance_count++;																	\
	//	inline static int instance_count = 0;														*/

#define MetaClassDefine