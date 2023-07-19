// ReSharper disable CppInconsistentNaming
#include "stdafx.h"
#include "display_window.h"
#include <mmsystem.h>
#include <ranges>

#include "actor_input_component.h"
#include "player_controller.h"
#include "BoxModel.h"
#include "camera.h"
#include "common.h"
#include "display_window_private.h"
#include "frame_rate_calc.h"
#include "IlluminantModel.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "LightBoxModel.h"
#include "shader.h"

#include "Model.h"
#include "input_defines.h"
#include "igui.h"
#include "magic_enum.hpp"

#pragma comment(lib,"Winmm.lib") 


DisplayWindow::DisplayWindow()
	:d_ptr(std::make_unique<DisplayWindowPrivate>(this))
	, data(new DisplayWindowData)
{
	Q_D(DisplayWindow);
	//d->init();
	d->initializeOpenGL();
	d->initGLFWWindow();
	d->initializeImGui();
}

DisplayWindow::~DisplayWindow()
{
	Q_D(DisplayWindow);
	//d->finish();
	d->destroyImGui();
	
}

int DisplayWindow::exec()
{
	Q_D(DisplayWindow);
	render_init();


	constexpr float targetFrameTime = 1.0f / 165.0f;
	float elapsedAccumulator = 0;

	d->beginTime = Clock::now();
	while (!glfwWindowShouldClose(d->window))
	{

		const TimePoint lastTime = std::exchange(d->currentTime, Clock::now());
		const Duration elapsed = d->currentTime - lastTime;
		elapsedAccumulator += elapsed.count();

		if (1||elapsedAccumulator >= targetFrameTime)
		{
			render_tick(elapsed.count());
			elapsedAccumulator = 0;
			//elapsedAccumulator -= targetFrameTime;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

	}
	render_exit();
	return 0;
}

void DisplayWindow::render_init()
{
	Q_D(DisplayWindow);
	
	int w, h;
	glfwGetWindowSize(d->window, &w, &h);


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	

	for(const auto& render: GlobalRenderList())
	{
		render->init(d->window);
	}
}

void DisplayWindow::render_exit()
{
	Q_D(DisplayWindow);
	//glDeleteBuffers(1, &d->VAO);
	//glDeleteBuffers(1, &d->VBO);
	//glDeleteBuffers(1, &d->EBO);
	for (const auto& render : GlobalRenderList())
	{
		render->exit();
	}
}

void DisplayWindow::render_tick(float deltaTime)
{
	processFrameBegin(deltaTime);
	processInput(deltaTime);
	processUpdate(deltaTime);
	processDraw(deltaTime);
	processFrameEnd(deltaTime);
}


void DisplayWindow::render_draw(float deltaTime)
{

	Q_D(DisplayWindow);
	//const float elapsed_sec = static_cast<float>(elapsed.count()) / static_cast<float>(_runTime::period::den);
	
	switch (d->draw_mode)
	{
	case 0:glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
	case 1:glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
	case 2:glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
	}


	// 渲染UI
	d->defaultImGuiDraw();

}

void DisplayWindow::processInput(float deltaTime)
{
	Q_D(DisplayWindow);
	
}

// ReSharper disable once CppMemberFunctionMayBeConst
void DisplayWindow::processUpdate(float deltaTime)
{
	
	for (const auto& render : GlobalRenderList()){
		render->tick(deltaTime);
	}
}

void DisplayWindow::processDraw(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (const auto& render : GlobalRenderList())
	{
		render->draw(deltaTime);
	}
	render_draw(deltaTime);
}

void DisplayWindow::processFrameBegin(float deltaTime)
{
	glfwPollEvents();
	d_func()->beginImGuiFrame();
}

void DisplayWindow::processFrameEnd(float deltaTime)
{
	d_func()->endImGuiFrame();
	glfwSwapBuffers(d_func()->window);
}

void DisplayWindow::resizeEvent(int width, int height)
{

	Q_D(DisplayWindow);
	
	for (const auto& render : GlobalRenderList()) {
		render->resizeEvent(width, height);
	}
}
template<typename EnumType>
auto FormatModifiers(flags<EnumType> mods)
{

	std::string result;
	DecomposeFlags(mods, [&](EnumType e)
		{
			if (!result.empty()) result += ", ";
			result += magic_enum::enum_name(e);
		});
	return result;
};
/**
 * @brief 处理键盘事件的回调函数。
 *
 * @param keyCode 按键的键码。
 * @param scanCode 按键的扫描码。
 * @param keyAction 按键的动作或状态。
 * @param keyModifiers 按键的修饰符。
 */
void DisplayWindow::keyEvent(int keyCode, int scanCode, int keyAction, int keyModifiers)
{
	
	
	Q_D(DisplayWindow);
	GLFWwindow* window = d->window;

	
	auto EmptyThen = [](std::string_view s, std::string_view then)
	{
		return s.empty() ? then : s;
	};

	//std::cout << std::format("keyCode({}),scanCode({}),keyAction({}),keyModifiers({})\n"
	//	, EmptyThen(magic_enum::enum_name(static_cast<EKeyCode>(keyCode)), std::to_string(keyCode))
	//	, scanCode
	//	, EmptyThen(magic_enum::enum_name(static_cast<EKeyAction>(keyAction)), std::to_string(keyAction))
	//	, FormatModifiers(EKeyMods(keyModifiers)));
	//	<< std::endl;
	//static int left, top, width, height;

	// 根据按键进行相应的处理逻辑
	switch (keyCode)
	{
	case GLFW_KEY_ESCAPE: 
		// 处理 ESC 按键
		if (keyAction == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			//SendMessageA(GetActiveWindow(), WM_CLOSE, 0, 0);
		}
		return;
	case GLFW_KEY_F10:
		// 处理 F10 按键
		if (keyAction == GLFW_PRESS || keyAction == GLFW_REPEAT)
		{
			// 切换全屏模式
			toggleFullscreenMode();
		}
		return;
	case GLFW_KEY_SLASH: 
		// 处理斜杠键按下事件
		if (keyAction == GLFW_RELEASE)
		{
			// 切换鼠标模式
			toggleMouseMode();
		}
		return;
	case GLFW_KEY_LEFT_BRACKET:
		// 处理左方括号键按下事件
		if (keyAction == GLFW_RELEASE)
		{
			d->draw_mode = (d->draw_mode + 1) % 3;
		}
		break;
	case GLFW_KEY_RIGHT_BRACKET: 
		// 处理右方括号键按下事件
		if (keyAction == GLFW_RELEASE)
		{
			d->draw_mode = (d->draw_mode - 1 + 3) % 3;
		}
		break;
	//case GLFW_KEY_LEFT_SHIFT: {
	//	if (action == GLFW_PRESS)
	//	{
	//		data->camera.processKeyAction(Camera::SPEED_UP);
	//		//data->camera.MovementSpeedRatio = d->speed_up_ratio;
	//	}
	//	else if (action == GLFW_RELEASE)
	//	{
	//		data->camera.MovementSpeedRatio = d->normal_speed_ratio;
	//	}
	//} break;
	default: {

	}break;
	}
	for (const auto& render : GlobalRenderList()) {
		render->keyEvent(keyCode, scanCode, keyAction, keyModifiers);
	}
}

void DisplayWindow::mouseButtonEvent(int buttonCode, int keyAction, int keyModifiers)
{
	for (const auto& render : GlobalRenderList()) {
		render->mouseButtonEvent(buttonCode, keyAction, keyModifiers);
	}
	
	switch (buttonCode)
	{
	case GLFW_MOUSE_BUTTON_LEFT:

		break;
	case GLFW_MOUSE_BUTTON_RIGHT:

		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:

		break;
	default:
		break;
	}
}

void DisplayWindow::mouseMoveEvent(float mouseX, float mouseY, float deltaX, float deltaY)
{
	for (const auto& render : GlobalRenderList()) {
		render->mouseMoveEvent(mouseX, mouseY, deltaX, deltaY);
	}
	
	Q_D(DisplayWindow);
	if (d->mouseMode == MouseMode::Enabled)return;
	
}

void DisplayWindow::scrollEvent(float deltaX, float deltaY)
{
	for (const auto& render : GlobalRenderList()) {
		render->scrollEvent(deltaX, deltaY);
	}
	
	Q_D(DisplayWindow);
	if (d->mouseMode == MouseMode::Enabled)return;
	
}

void DisplayWindow::toggleFullscreenMode()
{
	Q_D(DisplayWindow);
	GLFWwindow* window = d->window;
	auto& [left, top, width, height] = d->lastRect;

	if (glfwGetWindowMonitor(window) == NULL)
	{
		// 进入全屏模式
		
		glfwGetWindowPos(window, &left, &top);
		glfwGetWindowSize(window, &width, &height);
		const int centerX = left + width / 2;
		const int centerY = top + height / 2;
		// 找到中心点所在窗口
		GLFWmonitor* workMonitor = findMonitorByPosition(centerX, centerY);

		if(workMonitor!=nullptr)
		{
			workMonitor = glfwGetPrimaryMonitor();
		}

		const GLFWvidmode* vidMode = glfwGetVideoMode(workMonitor);
		glfwSetWindowMonitor(window, workMonitor, 0, 0, vidMode->width, vidMode->height, vidMode->refreshRate);
		glfwSwapInterval(1); // Enable vsync
	}
	else
	{
		// 退出全屏模式
		glfwSetWindowMonitor(window, NULL, left, top, width, height, GLFW_DONT_CARE);
	}
}

void DisplayWindow::toggleMouseMode()
{
	Q_D(DisplayWindow);
	GLFWwindow* window = d->window;
	if (d->mouseMode == MouseMode::Enabled)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		d->mouseMode = MouseMode::Disabled;
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		d->mouseMode = MouseMode::Enabled;
	}
}
// Find the monitor containing the specified position (centerX, centerY)
// and return the corresponding GLFWmonitor pointer.
// Returns nullptr if no monitor is found.
GLFWmonitor* DisplayWindow::findMonitorByPosition(int centerX, int centerY)
{
	int numMonitor;
	GLFWmonitor** monitors = glfwGetMonitors(&numMonitor);
	for (int i = 0; i < numMonitor; ++i)
	{
		GLFWmonitor* monitor = monitors[i];
		//WindowRect rect;  // NOLINT(cppcoreguidelines-pro-type-member-init)
		//glfwGetMonitorWorkarea(monitor, &rect.left, &rect.height, &rect.width, &rect.height);
		//if (rect.Contains(centerX, centerY))
		//{
		//	return monitor;
		//}

		// Retrieve the work area (position and size) of the monitor
		int xpos, ypos, width, height;
		glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);

		// Check if the position (centerX, centerY) is within the work area
		if (xpos <= centerX && centerX < xpos + width &&
			ypos <= centerY && centerY < ypos + height)
		{
			return monitor; // Found the monitor, return it
		}
	}
	return nullptr;
}
