#include "window_manager.h"
#include "display_window_private.h"

std::map<const GLFWwindow*, DisplayWindowPrivate*> WindowManager::_windowMap;
std::map<const GLFWwindow*, EventDispatcher*> WindowManager::_dispatcherMap;

DisplayWindowPrivate* WindowManager::GetWindowPrivate(const GLFWwindow* glfWwindow)
{
	const auto result = _windowMap.find(glfWwindow);
	if (result == _windowMap.end())
	{
		return result->second;
	}
	return nullptr;
}

EventDispatcher* WindowManager::GetEventDispatcher(const GLFWwindow* glfWwindow)
{
	const auto result = _windowMap.find(glfWwindow);
	if(result != _windowMap.end())
	{
		return &result->second->event_dispatcher;
	}
	return nullptr;
}

DisplayWindow* WindowManager::GetWindow(const GLFWwindow* glfWwindow)
{
	const auto result = _windowMap.find(glfWwindow);
	if (result != _windowMap.end())
	{
		return result->second->q_func();
	}
	return nullptr;
}
