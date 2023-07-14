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



CGLWindow::CGLWindow()
	:d_ptr(std::make_unique<CGLWindowPrivate>(this))
	, data(new CGLWindowData)
{
	Q_D(CGLWindow);
	d->init();
}

CGLWindow::~CGLWindow()
{
	Q_D(CGLWindow);
	d->finish();
	glfwTerminate();
}

int CGLWindow::exec()
{
	Q_D(CGLWindow);
	render_init();

	GLFWwindow* window = d->window;

	constexpr float targetFrameTime = 1.0f / 165.0f;
	float elapsedAccumulator = 0;
	FrameRateCalculator frc;

	d->beginTime = Clock::now();
	while (!glfwWindowShouldClose(window))
	{

		const TimePoint lastTime = std::exchange(d->currentTime, Clock::now());
		const Duration elapsed = d->currentTime - lastTime;
		elapsedAccumulator += elapsed.count();

		if (1||elapsedAccumulator >= targetFrameTime)
		{
			processFrameBegin(targetFrameTime);
			processInput(targetFrameTime);
			processUpdate(targetFrameTime);
			processDraw(targetFrameTime);
			processFrameEnd(targetFrameTime);
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

void CGLWindow::render_init()
{
	Q_D(CGLWindow);
	
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

void CGLWindow::render_exit()
{
	Q_D(CGLWindow);
	//glDeleteBuffers(1, &d->VAO);
	//glDeleteBuffers(1, &d->VBO);
	//glDeleteBuffers(1, &d->EBO);

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
void CGLWindow::render_draw(float deltaTime)
{

	Q_D(CGLWindow);
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
	d->ImGuiDefaultDraw();

}

void CGLWindow::processInput(float deltaTime)
{
	Q_D(CGLWindow);
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
void CGLWindow::processUpdate(float deltaTime)
{
	data->camera.update(deltaTime);
}

void CGLWindow::processDraw(float deltaTime)
{
	render_draw(deltaTime);
}

void CGLWindow::processFrameBegin(float deltaTime)
{
	glfwPollEvents();
	d_func()->ImGuiFrameBegin();
}

void CGLWindow::processFrameEnd(float deltaTime)
{
	d_func()->ImGuiFrameEnd();
	glfwSwapBuffers(d_func()->window);
}

void CGLWindow::on_resize(int width, int height)
{

	Q_D(CGLWindow);
	if (width > 0 && height > 0)
	{
		data->camera.resizeViewport(width, height);
	}
}

void CGLWindow::on_key(int key, int scan_code, int action, int mods)
{
	Q_D(CGLWindow);
	GLFWwindow* window = d->window;
	//std::cout << std::format("key({}),code({}),action({}),mode({})", key, scan_code, action, mods)
	//	<< std::endl;
	static int left, top, width, height;
	switch (key)
	{
	case GLFW_KEY_ESCAPE: {
		if (action == GLFW_PRESS)
			SendMessageA(GetActiveWindow(), WM_CLOSE, 0, 0);
	} break;
	case GLFW_KEY_F10: {
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			if (glfwGetWindowMonitor(window) == NULL)
			{
				int numMonitor;
				GLFWmonitor** monitors = glfwGetMonitors(&numMonitor);
				glfwGetWindowPos(window, &left, &top);
				glfwGetWindowSize(window, &width, &height);
				const int center_x = left + width / 2;
				const int center_y = top + height / 2;
				for (int i = 0; i < numMonitor; ++i)
				{
					GLFWmonitor* monitor = monitors[i];
					int xpos, ypos, width, height;
					glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);
					if (center_x >= xpos && center_x < xpos + width && center_y >= ypos && center_y < ypos + height)
					{
						const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
						glfwSetWindowMonitor(window, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
						glfwSwapInterval(1); // Enable vsync
						break;
					}
				}
			}
			else
			{
				glfwSetWindowMonitor(window, NULL, left, top, width, height, GLFW_DONT_CARE);
			}
		}
	} break;
	case GLFW_KEY_SLASH: {
		if (action == GLFW_RELEASE)
		{
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
	} break;
	case GLFW_KEY_LEFT_BRACKET: {
		if (action == GLFW_RELEASE)
		{
			d->draw_mode = (d->draw_mode + 1) % 3;
		}
	} break;
	case GLFW_KEY_RIGHT_BRACKET: {
		if (action == GLFW_RELEASE)
		{
			d->draw_mode = (d->draw_mode - 1 + 3) % 3;
		}
	} break;
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

void CGLWindow::on_mouseMove(float x_pos, float y_pos, float x_offset, float y_offset)
{
	Q_D(CGLWindow);
	if (d->mouseMode == MouseMode::Enabled)return;
	data->camera.processMouseMove(x_offset, -y_offset);
}

void CGLWindow::on_scroll(float x_offset, float y_offset)
{
	Q_D(CGLWindow);
	if (d->mouseMode == MouseMode::Enabled)return;
	data->camera.processMouseScroll(y_offset);
}

