#include "object.h"


std::map<const object*, std::vector<std::shared_ptr<Delegate_Handle_Base>>> object_handles;

std::string_view object::get_type_name(object* obj)
{
	const std::string_view name(typeid(*obj).name());
	const auto pos = name.find_last_of(' ');
	if (pos != std::string_view::npos)
	{
		return name.substr(pos + 1);
	}
	return name;
}
