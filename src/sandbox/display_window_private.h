#pragma once

#include "imgui.h"
#include "display_window.h"
#include "common.h"
#include "shader.h"
#include "vertices.h"
#include "camera.h"
#include "frame_rate_calc.h"


#include <ranges>

#include "window_manager.h"

// ����


// ���ھ��ζ���
struct WindowRect
{
	int left;
	int top;
	int width;
	int height;

	// �����Ƿ�����ڴ�����
	bool Contains(int x, int y) const;
};


class DisplayWindowPrivate
{
	Q_DECLARE_PUBLIC(DisplayWindow)
	friend struct WindowManager;
public:
	explicit DisplayWindowPrivate(DisplayWindow* parent)
		: q_ptr(parent),
		  mt(std::random_device("")()),
		  beginTime(DisplayWindow::Clock::now())
		  , currentTime(beginTime), wMouseX(0), wMouseY(0), lastMouseX(0), lastMouseY(0), lastRect()
	{
	}

	// ����������ʱ������glfw���ڣ�������д��ڶ������ˣ�����glfwTerminate
	~DisplayWindowPrivate()
	{
		WindowManagerInstance->_windowMap.erase(window);
		glfwDestroyWindow(window);
		if(WindowManagerInstance->_windowMap.empty())
		{
			//glfwTerminate();
			openglInitialized = false;
		}
	}

	// ��ʼ��Opengl��أ�����������������ֻ��ִ��һ��
	static void initializeOpenGL();

	void initGLFWWindow();
	auto createGLFWWindow() -> GLFWwindow*;

	void initializeImGui(); // ��Ҫ�ȵ���initGLFWWindow
	void defaultImGuiDraw();
	void beginImGuiFrame(); // Start the Dear ImGui frame
	void endImGuiFrame(); // Rendering
	void destroyImGui();


	// ���º�������glfw�¼����ã����ַ���Own������
	static void handleErrorEvent(int error, const char* description);
	static void handleFrameBufferResizeEvent(GLFWwindow* window, int width, int height);
	static void handleCloseEvent(GLFWwindow* glfwWindow);
	static void handleKeyEvent(GLFWwindow* window, int key, int scanCode, int action, int mods);
	static void handleMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
	static void handleMouseMoveEvent(GLFWwindow* window, double newX, double newY);
	static void handleScrollEvent(GLFWwindow* window, double deltaX, double deltaY);


	// ÿ��glfwWindow��Ӧ��DisplayWindow
	inline static bool openglInitialized = false;
	inline static bool gladLoaded = false;
	DisplayWindow* q_ptr;

	EventDispatcher event_dispatcher;

	std::default_random_engine mt;
	DisplayWindow::TimePoint beginTime;
	DisplayWindow::TimePoint currentTime;

	GLFWwindow* const window = nullptr;

	// Ĭ�ϵ�Dear ImGui����
	bool show_demo_window = true;
	bool show_another_window = false;
	int draw_mode = 0;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	DisplayWindow::MouseMode mouseMode = DisplayWindow::MouseMode::Disabled;

	bool bIgnoreMoveEvent = false;
	int nIgnoreMoveEventTimes = 0;


	double wMouseX, wMouseY;
	//
	bool isFirst = false;
	double lastMouseX, lastMouseY;


	// ���ȫ��֮ǰ�Ĵ���λ�ô�С
	WindowRect lastRect;
};
