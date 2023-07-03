// ReSharper disable CppInconsistentNaming
#include "stdafx.h"
#include "display_window.h"
#include <mmsystem.h>
#include <ranges>
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

	data->camera.Position = { 0,0,3.f };
	data->camera.ZFar = 1000;
	data->camera.resizeViewport(w, h);
	data->camera.MovementSpeed = 2.5;
	data->camera.MouseSensitivity = CAMERA_DEFAULT_SENSITIVITY * 0.7f;
	data->boxModel = comm::getOrCreate<BoxModel>();
	data->lightModel = comm::getOrCreate<LightBoxModel>();
	data->illuminantModel = comm::getOrCreate<IlluminantModel>();
	data->lightModel2 = comm::getOrCreate<LightBoxModel2>();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	auto materials = MaterialTable::instance()->getMaterials();
	for (int i = 0; i < 100; ++i)
	{
		glm::vec3 pos;
		const double scale = std::uniform_real_distribution<>(0.78, 5.0)(d->mt);
		const auto n = std::uniform_int_distribution<>(0, static_cast<int>(materials.size()) - 1)(d->mt);
		std::uniform_real_distribution<> urd(10, 50);
		std::uniform_int_distribution<> uid(0, 1);// 中间清理出一块空地
		pos = { urd(d->mt) * (uid(d->mt) ? 1 : -1), (scale - 1) / 2, urd(d->mt) * (uid(d->mt) ? 1 : -1) };
		data->boxArguments.emplace_back(std::make_tuple(pos, scale, materials[n]));
	}


}

void DisplayWindow::render_exit()
{
	Q_D(DisplayWindow);
	//glDeleteBuffers(1, &d->VAO);
	//glDeleteBuffers(1, &d->VBO);
	//glDeleteBuffers(1, &d->EBO);

}

void DisplayWindow::render_tick(float deltaTime)
{
	processFrameBegin(deltaTime);
	processInput(deltaTime);
	processUpdate(deltaTime);
	processDraw(deltaTime);
	processFrameEnd(deltaTime);
}

static void HelpMarker(const char* desc)
{
	//ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
void DisplayWindow::render_draw(float deltaTime)
{

	Q_D(DisplayWindow);
	//const float elapsed_sec = static_cast<float>(elapsed.count()) / static_cast<float>(_runTime::period::den);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	switch (d->draw_mode)
	{
	case 0:glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
	case 1:glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
	case 2:glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
	}

	const glm::mat4 view = data->camera.getViewMatrix();
	const glm::mat4 proj = data->camera.getProjMatrix();
	//d->shader->glUniform("view", view);
	//d->boxModel->begin();
	//for(int i = 0;i < cubePositions_size;++i)
	//{
	//    //glm::mat4 model = glm::mat4(1.0f);
	//
	//    glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
	//    model = glm::rotate(model, glm::radians(20.f* i), glm::vec3(1, 0.3, 0.5));
	//    
	//    d->boxModel->drawAt(view, d->projection, model);
	//}
	ImGui::Begin("Light");
	static glm::vec3 lightColor(1, 1, 1);
	static glm::vec3 lightRatio(0.3f, 0.7f, 1);
	//lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0)*0.5 + 0.5);
	//lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7)*0.5 + 0.5);
	//lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3)*0.5 + 0.5);
	ImGui::ColorEdit3("Light Color", &lightColor.x);
	ImGui::DragFloat3("Light Ratio", &lightRatio.x, 0.01,0,10,"%.3f");
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	HelpMarker(
		"Component proportion of light source. \nThese are in order: \n[1]ambient \n[2]diffuse \n[3]specular");


	static float velocity = 15.0f;
	static float angle = 0;
	angle = NormalizeAngleDegrees(angle + velocity * deltaTime);

	const glm::mat4 rotation = glm::rotate(glm::mat4(1),
	                                       glm::radians(angle),
	                                       glm::vec3(0, 1, 0));
	glm::vec3 lightWorldPos = rotation * glm::vec4{ 1.2f, 5.2f, 10.0f,1.0f } *2.0f;


	ImGui::DragFloat("Moving Speed:", &velocity, 1, -1440, 1440);



	// 光源
	Light light = { lightColor * lightRatio.x,lightColor * lightRatio.y,lightColor* lightRatio.z, lightWorldPos };
	

	// 创建发光体
	{
		// 灯光模型的变换矩阵
		glm::mat4 lightWorldMatrix(1.0f);
		// 设置位置
		lightWorldMatrix[3] = { light.position,1 };
		// 先缩放模型大小
		lightWorldMatrix = glm::scale(lightWorldMatrix, glm::vec3(0.2f));
		data->illuminantModel->setLightColor(light.specular);
		data->illuminantModel->draw(data->camera, lightWorldMatrix);
	}
	//随机位置生成盒子
	{
		data->lightModel2->setLight(light);
		auto wMatsView = data->boxArguments | std::ranges::views::transform([](auto&& tuple) {
			return glm::scale(glm::translate(glm::mat4(1.0f), std::get<0>(tuple) + glm::vec3{ 0,0.0001,0 }), glm::vec3(static_cast<float>(std::get<1>(tuple))));
			});
		data->lightModel2->draw(data->camera, { wMatsView.begin(),wMatsView.end() });
	}

	{
		//d->lightModel2->setLight(light);
		//d->lightModel2->draw(data->camera, glm::translate(glm::mat4(1.0f),{0,0,1.0f} ));
	}

	// 地面
	{
		data->lightModel->setLight(light);
		glm::mat4 wMat(1.0f);
		wMat[3] = { 0.0f,-1.0f,0.0f,wMat[3][3] };
		wMat = glm::scale(wMat, { 150.0f,1.f,150.0f });
		data->lightModel->setMaterial(MaterialTable::instance()->getMaterial("default"));
		data->lightModel->draw(data->camera, wMat);
	}
	{
		static auto shader = ShaderProgram::makeShaderByName("lightingShader.vert", "lightingShader3.frag");
		static Model model(R"(C:/Users/zhang/Pictures/Material/nanosuit/nanosuit.obj)");
		shader->use();
		shader->glUniform("lights[0].type", 1);
		shader->glUniform("viewPos", data->camera.Position);
		shader->glUniform("lights[0].position", light.position);
		shader->glUniform("lights[0].ambient", light.ambient);
		shader->glUniform("lights[0].diffuse", light.diffuse);
		shader->glUniform("lights[0].specular", light.specular);
		shader->glUniform("lights[0].constant", 1.0f);
		shader->glUniform("lights[0].linear", 0.f);
		shader->glUniform("lights[0].quadratic", 0.f);
		shader->glUniform("material.shininess", 32.0f);
		shader->glUniform("view", data->camera.getViewMatrix());
		shader->glUniform("projection", data->camera.getProjMatrix());
		shader->glUniform("model", glm::scale(glm::mat4(1.0f), glm::vec3{ 0.5f,0.5f,0.5f }*2.0f));
		model.Draw(*shader);
	}

	

	ImGui::Text("%s", std::format("Light Position: {:.3f}, {:.3f}, {:.3f}", light.position.x, light.position.y, light.position.z).c_str());
	
	ImGui::End();


	// 渲染UI
	d->defaultImGuiDraw();

}

void DisplayWindow::processInput(float deltaTime)
{
	Q_D(DisplayWindow);
	Camera::Camera_KeyActions cameraKeyActions(Camera::IDLE);
	if (glfwGetKey(d->window, GLFW_KEY_W) == GLFW_PRESS)
		cameraKeyActions |= Camera::FORWARD;
	if (glfwGetKey(d->window, GLFW_KEY_S) == GLFW_PRESS)
		cameraKeyActions |= Camera::BACKWARD;
	if (glfwGetKey(d->window, GLFW_KEY_A) == GLFW_PRESS)
		cameraKeyActions |= Camera::LEFT;
	if (glfwGetKey(d->window, GLFW_KEY_D) == GLFW_PRESS)
		cameraKeyActions |= Camera::RIGHT;
	if (glfwGetKey(d->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraKeyActions |= Camera::SPEED_UP;
	data->camera.processKeyAction(cameraKeyActions);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void DisplayWindow::processUpdate(float deltaTime)
{
	data->camera.update(deltaTime);
}

void DisplayWindow::processDraw(float deltaTime)
{
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
	if (width > 0 && height > 0)
	{
		data->camera.resizeViewport(width, height);
	}
}

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
	//std::cout << std::format("key({}),code({}),action({}),mode({})", key, scan_code, action, mods)
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
		break;
	case GLFW_KEY_F10:
		// 处理 F10 按键
		if (keyAction == GLFW_PRESS || keyAction == GLFW_REPEAT)
		{
			// 切换全屏模式
			toggleFullscreenMode();
		}
		break;
	case GLFW_KEY_SLASH: 
		// 处理斜杠键按下事件
		if (keyAction == GLFW_RELEASE)
		{
			// 切换鼠标模式
			toggleMouseMode();
		}
		break;
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

}

void DisplayWindow::mouseMoveEvent(float mouseX, float mouseY, float deltaX, float deltaY)
{
	Q_D(DisplayWindow);
	if (d->mouseMode == MouseMode::Enabled)return;
	data->camera.processMouseMove(deltaX, -deltaY);
}

void DisplayWindow::scrollEvent(float deltaX, float deltaY)
{
	Q_D(DisplayWindow);
	if (d->mouseMode == MouseMode::Enabled)return;
	data->camera.processMouseScroll(deltaY);
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
