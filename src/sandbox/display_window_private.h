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
public:
	explicit DisplayWindowPrivate(DisplayWindow* parent)
		: q_ptr(parent),
		mt(std::random_device()()),
		beginTime(DisplayWindow::Clock::now())
		, currentTime(beginTime)
	{
		
	}

	// ����������ʱ������glfw���ڣ�������д��ڶ������ˣ�����glfwTerminate
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
	static std::map<GLFWwindow*, DisplayWindow*> glfwWindowMap;
	inline static bool openglInitialized = false;
	inline static bool gladLoaded = false;
	DisplayWindow* q_ptr;


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

	//
	bool isFirst = false;
	double lastMouseX, lastMouseY;


	// ���ȫ��֮ǰ�Ĵ���λ�ô�С
	WindowRect lastRect;
};
