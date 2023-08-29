#pragma once
#include <map>
#include "common.h"
#include "event_dispatcher.h"

class DisplayWindowPrivate;
class GLFWwindow;
class DisplayWindow;
struct DisplayWindowData;


struct WindowManager
{
	static DisplayWindowPrivate* GetWindowPrivate(const GLFWwindow*);
	static EventDispatcher* GetEventDispatcher(const GLFWwindow*);
	static DisplayWindow* GetWindow(const GLFWwindow*);

	static std::map<const GLFWwindow*, DisplayWindowPrivate*> _windowMap;
	static std::map<const GLFWwindow*, EventDispatcher*> _dispatcherMap;
};

inline auto WindowManagerInstance = comm::getOrCreate<WindowManager>();