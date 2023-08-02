#include "input_manager.h"

#include <format>
#include <iostream>


std::map<std::string, InputActionKeyMapping>& GetInputActionKeyMappings()
{
	static std::map<std::string, InputActionKeyMapping> g_InputActionKeyMappings;
	return g_InputActionKeyMappings;
}

std::map<std::string, InputAxisKeyMapping>& GetInputAxisKeyMappings()
{
	static std::map<std::string, InputAxisKeyMapping> g_InputAxisKeyMappings;
	return g_InputAxisKeyMappings;
}

void InputManager::EnableInput()
{
	_bEnableInput = true;
}

void InputManager::DisableInput()
{
	_bEnableInput = false;
}

EKeyAction InputManager::GetKeyState(EKeyCode code) const
{
	if(code < EKeyCode::k_PhysicalKeyEnd)
	{
		if(code >= EKeyCode::K_Space)
		{
			return static_cast<EKeyAction>(glfwGetKey(_window, static_cast<int>(code)));
		}else if(code <= EKeyCode::k_MouseButtonLast)
		{
			return static_cast<EKeyAction>(glfwGetMouseButton(_window, static_cast<int>(code)));
		}
	}
	return EKeyAction::A_Release;
}

void InputManager::keyEvent(int keyCode, int keyAction, int keyModifiers)
{
	if (!_bEnableInput)return;
	_keyEvents.emplace(static_cast<EKeyCode>(keyCode) , static_cast<EKeyAction>(keyAction), static_cast<EKeyMods>(keyModifiers));
}

void InputManager::mouseButtonEvent(int buttonCode, int keyAction, int keyModifiers)
{
	if (!_bEnableInput)return;
	_keyEvents.emplace(static_cast<EKeyCode>(buttonCode), static_cast<EKeyAction>(keyAction), static_cast<EKeyMods>(keyModifiers));
}

void InputManager::mouseMoveEvent(float deltaX, float deltaY)
{
	if (!_bEnableInput)return;
	_mouseMoveEvents.emplace(deltaX, deltaY);
}

void InputManager::scrollEvent(float deltaX, float deltaY)
{
	if (!_bEnableInput)return;
	_scrollEvents.emplace(deltaX, deltaY);
}

template<typename T, typename CallBack>
requires (std::is_base_of_v<std::deque<typename T::value_type>, T>
		|| std::is_base_of_v<std::queue<typename T::value_type>, T>)
	&&std::is_invocable_v<CallBack, typename T::value_type&>
auto expend(T &container, CallBack cb)
{
	cb(container.back());
	container.pop();
}
template<typename T, typename CallBack>
	requires (std::is_base_of_v<std::deque<typename T::value_type>, T>
|| std::is_base_of_v<std::queue<typename T::value_type>, T>)
	&& std::is_invocable_v<CallBack, typename T::value_type&>
	auto expendAll(T& container, CallBack cb)
{
	while(!container.empty())
	{
		cb(container.back());
		container.pop();
	}
}



template<typename T>
	requires requires (std::remove_cv_t<T>& val, std::remove_cv_t<T> arg) { val.swap(arg); }
void clear(T& container)
{
	std::remove_cv_t<T> temp{};
	container.swap(temp);
}

void InputManager::tick(float deltaTime)
{
	if(_keyEvents.empty()&&_mouseMoveEvents.empty()&&_scrollEvents.empty())
		return;
	//std::cout << std::format(
	//	"keyEvents: {}, "
	//	"mouseButtonEvents: {}, "
	//	"mouseMoveEvents: {}, "
	//	"scrollEvents: {}"
	//	"\n"
	//	, _keyEvents.size()
	//	, _mouseMoveEvents.size()
	//	, _scrollEvents.size()
	//);
	
	processEvent();
	processAxis();

	if (!_keyEvents.empty())clear(_keyEvents);
	if (!_mouseMoveEvents.empty())clear(_mouseMoveEvents);
	if (!_scrollEvents.empty())clear(_scrollEvents);
}

void InputManager::processEvent()
{
	expendAll(_keyEvents, EventProcessor);

}

void InputManager::processAxis()
{
	expendAll(_mouseMoveEvents, MouseMoveAxisProcessor);
	expendAll(_scrollEvents, ScrollAxisProcessor);
}

void InputManager::SetWindow(GLFWwindow* window)
{
	_window = window;
}

GLFWwindow* InputManager::GetWindow() const
{
	return _window;
}

InputManager::Axis InputManager::mergeAxis(std::queue<Axis>& axes)
{
	Axis _axis{0,0};
	expendAll(axes, [&](const Axis& axis)
		{
			_axis.deltaX += axis.deltaX;
			_axis.deltaY += axis.deltaY;
		});
	return _axis;
}


