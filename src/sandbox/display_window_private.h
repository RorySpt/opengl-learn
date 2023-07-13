#pragma once

#include "imgui.h"
#include "display_window.h"
#include "common.h"
#include "shader.h"
#include "vertices.h"
#include "camera.h"
#include "frame_rate_calc.h"
#include "BoxModel.h"
#include "LightBoxModel.h"
#include "IlluminantModel.h"

#include <ranges>

// 好乱


// 窗口矩形定义
struct WindowRect
{
	int left;
	int top;
	int width;
	int height;

	// 坐标是否包含在窗口里
	bool Contains(int x, int y) const;
};



class DisplayWindowPrivate
{
	Q_DECLARE_PUBLIC(DisplayWindow)
public:
	explicit DisplayWindowPrivate(DisplayWindow* parent)
		: q_ptr(parent),
		mt(std::random_device()()),
		beginTime(DisplayWindow::Clock::now())
		, currentTime(beginTime)
	{
		
	}

	// 当窗口销毁时，销毁glfw窗口，如果所有窗口都销毁了，调用glfwTerminate
	~DisplayWindowPrivate()
	{
		glfwWindowMap.erase(window);
		glfwDestroyWindow(window);
		if(glfwWindowMap.empty())
		{
			glfwTerminate();
			openglInitialized = false;
		}
	}

	// 初始化Opengl相关，整个程序生命周期只会执行一次
	static void initializeOpenGL();

	void initGLFWWindow();
	auto createGLFWWindow() -> GLFWwindow*;

	void initializeImGui(); // 需要先调用initGLFWWindow
	void defaultImGuiDraw();
	void beginImGuiFrame(); // Start the Dear ImGui frame
	void endImGuiFrame(); // Rendering
	void destroyImGui();


	// 以下函数将由glfw事件调用，并分发给Own对象函数
	static void handleErrorEvent(int error, const char* description);
	static void handleFrameBufferResizeEvent(GLFWwindow* window, int width, int height);
	static void handleCloseEvent(GLFWwindow* glfwWindow);
	static void handleKeyEvent(GLFWwindow* window, int key, int scanCode, int action, int mods);
	static void handleMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
	static void handleMouseMoveEvent(GLFWwindow* window, double newX, double newY);
	static void handleScrollEvent(GLFWwindow* window, double deltaX, double deltaY);
	

	// 每个glfwWindow对应的DisplayWindow
	static std::map<GLFWwindow*, DisplayWindow*> glfwWindowMap;
	inline static bool openglInitialized = false;
	inline static bool gladLoaded = false;
	DisplayWindow* q_ptr;


	std::default_random_engine mt;
	DisplayWindow::TimePoint beginTime;
	DisplayWindow::TimePoint currentTime;

	GLFWwindow* const window = nullptr;

	// 默认的Dear ImGui设置
	bool show_demo_window = true;
	bool show_another_window = false;
	int draw_mode = 0;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	DisplayWindow::MouseMode mouseMode = DisplayWindow::MouseMode::Disabled;

	//
	bool isFirst = false;
	double lastMouseX, lastMouseY;


	// 鼠标全屏之前的窗口位置大小
	WindowRect lastRect;
};
