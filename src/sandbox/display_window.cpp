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
#include "shader.h"

#include "Model.h"
#include "input_defines.h"
#include "igui.h"
#include "magic_enum.hpp"

#pragma comment(lib,"Winmm.lib") 

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

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
			std::this_thread::yield();
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
	d->wMouseX = w / 2;
	d->wMouseY = h / 2;

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	

	for(const auto& render: GlobalRenderList())
	{
		render->init(d->window);
	}
}
extern std::map<std::string, unsigned int> s_textureMap;
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
	std::vector<unsigned> vector;
	vector.reserve(s_textureMap.size());
	for (const auto& render : s_textureMap)
	{
		vector.emplace_back(render.second);
	}
	glDeleteTextures(vector.size(), vector.data());
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


	// ��ȾUI
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
	d->event_dispatcher.resizeHandler.cast({ width,height });

	for (const auto& render : GlobalRenderList()) {
		render->resizeEvent(width, height);
	}
	constexpr auto s = magic_enum::detail::values<EKeyAction, magic_enum::detail::enum_subtype::common>();
	constexpr auto ss = magic_enum::enum_name(EKeyCode::K_Menu);
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
 * @brief ��������¼��Ļص�������
 *
 * @param keyCode �����ļ��롣
 * @param scanCode ������ɨ���롣
 * @param keyAction �����Ķ�����״̬��
 * @param keyModifiers ���������η���
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

	// ���ݰ���������Ӧ�Ĵ����߼�
	switch (keyCode)
	{
	case GLFW_KEY_ESCAPE: 
		// ���� ESC ����
		if (keyAction == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			//SendMessageA(GetActiveWindow(), WM_CLOSE, 0, 0);
		}
		return;
	case GLFW_KEY_F10:
		// ���� F10 ����
		if (keyAction == GLFW_PRESS || keyAction == GLFW_REPEAT)
		{
			// �л�ȫ��ģʽ
			toggleFullscreenMode();
		}
		return;
	case GLFW_KEY_SLASH: 
		// ����б�ܼ������¼�
		if (keyAction == GLFW_RELEASE)
		{
			// �л����ģʽ
			toggleMouseMode();
		}
		return;
	case GLFW_KEY_LEFT_BRACKET:
		// ���������ż������¼�
		if (keyAction == GLFW_RELEASE)
		{
			d->draw_mode = (d->draw_mode + 1) % 3;
		}
		break;
	case GLFW_KEY_RIGHT_BRACKET: 
		// �����ҷ����ż������¼�
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

	d->event_dispatcher.keyHandler.cast({ keyCode, keyAction, keyModifiers });

	for (const auto& render : GlobalRenderList()) {
		render->keyEvent(keyCode, scanCode, keyAction, keyModifiers);
	}
}

void DisplayWindow::mouseButtonEvent(int buttonCode, int keyAction, int keyModifiers)
{
	Q_D(DisplayWindow);
	d->event_dispatcher.keyHandler.cast({ buttonCode, keyAction, keyModifiers });

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
	Q_D(DisplayWindow);
	if (d->mouseMode == MouseMode::Enabled || consumeNextMouseMoveEventIgnore())return;


	d->event_dispatcher.mouseMoveHandler.cast({ mouseX,mouseY,deltaX,deltaY });
	for (const auto& render : GlobalRenderList()) {
		render->mouseMoveEvent(mouseX, mouseY, deltaX, deltaY);
	}
}

void DisplayWindow::scrollEvent(float deltaX, float deltaY)
{
	Q_D(DisplayWindow);
	if (d->mouseMode == MouseMode::Enabled || consumeNextMouseMoveEventIgnore())return;

	d->event_dispatcher.scrollHandler.cast({ deltaX,deltaY });
	for (const auto& render : GlobalRenderList()) {
		render->scrollEvent(deltaX, deltaY);
	}
}

void DisplayWindow::toggleFullscreenMode()
{
	Q_D(DisplayWindow);
	GLFWwindow* window = d->window;
	auto& [left, top, width, height] = d->lastRect;

	if (glfwGetWindowMonitor(window) == NULL)
	{
		ignoreNextMouseMoveEvent();
		// ����ȫ��ģʽ
		
		glfwGetWindowPos(window, &left, &top);
		glfwGetWindowSize(window, &width, &height);
		const int centerX = left + width / 2;
		const int centerY = top + height / 2;
		// �ҵ����ĵ����ڴ���
		GLFWmonitor* workMonitor = findMonitorByPosition(centerX, centerY);

		if(workMonitor==nullptr)
		{
			workMonitor = glfwGetPrimaryMonitor();
		}

		const GLFWvidmode* vidMode = glfwGetVideoMode(workMonitor);
		glfwSetWindowMonitor(window, workMonitor, 0, 0, vidMode->width, vidMode->height, vidMode->refreshRate);
		glfwSwapInterval(1); // Enable vsync
	}
	else
	{
		ignoreNextMouseMoveEvent();
		// �˳�ȫ��ģʽ
		glfwSetWindowMonitor(window, NULL, left, top, width, height, GLFW_DONT_CARE);

	}
}

void DisplayWindow::toggleMouseMode()
{
	
	Q_D(DisplayWindow);
	GLFWwindow* window = d->window;
	if (d->mouseMode == MouseMode::Enabled)
	{
		ignoreNextMouseMoveEvent(); ignoreNextMouseMoveEvent();
		glfwGetCursorPos(d->window, &d->wMouseX, &d->wMouseY);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		d->mouseMode = MouseMode::Disabled;
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		d->mouseMode = MouseMode::Enabled;

		ignoreNextMouseMoveEvent(); ignoreNextMouseMoveEvent();
		glfwSetCursorPos(d->window, d->wMouseX,d->wMouseY);
	}

}

void DisplayWindow::transferMousePosition(float mouseX, float mouseY)
{
}

void DisplayWindow::enableMouseMoveEvent()
{
	d_func()->bIgnoreMoveEvent = true;
}

void DisplayWindow::disableMouseMoveEvent()
{
	d_func()->bIgnoreMoveEvent = false;
}

void DisplayWindow::ignoreNextMouseMoveEvent()
{
	++d_func()->nIgnoreMoveEventTimes;
}

bool DisplayWindow::consumeNextMouseMoveEventIgnore()
{
	if (d_func()->nIgnoreMoveEventTimes)
	{
		--d_func()->nIgnoreMoveEventTimes;
		return true;
	}

	return false;
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
