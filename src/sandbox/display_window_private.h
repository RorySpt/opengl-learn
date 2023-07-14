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
class CGLWindowPrivate
{
	Q_DECLARE_PUBLIC(CGLWindow)
public:
	explicit CGLWindowPrivate(CGLWindow* parent)
		: q_ptr(parent),
		mt(std::random_device()()),
		beginTime(CGLWindow::Clock::now())
		, currentTime(beginTime)
	{

	}
	void init();
	void update(); // maybe?
	void finish();
private:
	void initGLFW();

	void ImGuiInit();
	void ImGuiDefaultDraw();
	void ImGuiFrameBegin();
	void ImGuiFrameEnd();
	void ImGuiExit();

	// These will invoke by glfw event. And pass it on to q-object func
	static void cb_frame_buffer_size(GLFWwindow* window, int width, int height);
	static void cb_error(int error, const char* description);
	static void cb_close(GLFWwindow* glfwWindow);	static void cb_key(GLFWwindow* window, int key, int scanCode, int action, int mods);

	static void cb_mouse_move(GLFWwindow* window, double x_pos, double y_pos);
	static void cb_scroll(GLFWwindow* window, double x_offset, double y_offset);

	static std::map<GLFWwindow*, CGLWindow*> map_glfw_glw;
private:
	CGLWindow* q_ptr;


	std::default_random_engine mt;
	CGLWindow::TimePoint beginTime;
	CGLWindow::TimePoint currentTime;

	GLFWwindow* const window = nullptr;

	// Default DearImGui
	bool show_demo_window = true;
	bool show_another_window = false;
	int draw_mode = 0;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	CGLWindow::MouseMode mouseMode = CGLWindow::MouseMode::Disabled;

};
