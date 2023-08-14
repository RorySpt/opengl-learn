#include "stdafx.h"
#include "demo0_rs.h"

#include "Model.h"
#include "player_controller.h"


Demo0_RS::Demo0_RS()
	:mt(std::random_device()())
{
}

Demo0_RS::~Demo0_RS()
{
}

void Demo0_RS::init(GLFWwindow* window)
{
	_window = window;
	std::cout << __func__ << "\n";

	int w, h;
	glfwGetWindowSize(_window, &w, &h);

	camera.Position = { 0,0,3.f };
	camera.ZFar = 1000;
	camera.resizeViewport(w, h);
	camera.MovementSpeed = 2.5;
	camera.MouseSensitivity = CAMERA_DEFAULT_SENSITIVITY * 0.7f;
	lightModel = comm::getOrCreate<BoxModel_SingleColor>();
	illuminantModel = comm::getOrCreate<IlluminantModel>();
	lightModel2 = comm::getOrCreate<BoxModel_SimpleTexture>();

	auto materials = MaterialTable::instance()->getMaterials();
	for (int i = 0; i < 100; ++i)
	{
		glm::vec3 pos;
		const double scale = std::uniform_real_distribution<>(0.78, 5.0)(mt);
		const auto n = std::uniform_int_distribution<>(0, static_cast<int>(materials.size()) - 1)(mt);
		std::uniform_real_distribution<> urd(10, 50);
		std::uniform_int_distribution<> uid(0, 1);// 中间清理出一块空地
		pos = { urd(mt) * (uid(mt) ? 1 : -1), (scale - 1) / 2, urd(mt) * (uid(mt) ? 1 : -1) };
		boxArguments.emplace_back(std::make_tuple(pos, scale, materials[n]));
	}

	world.BeginPlay();
	input = &world.GetPlayerController()->_input_component->inputManager;
	input->SetWindow(window);
	input->EnableInput();

}

void Demo0_RS::exit()
{
	std::cout << __func__ << "\n";
	world.EndPlay();
}

void Demo0_RS::tick(float deltaTime)
{
	static bool first = false;
	if (!std::exchange(first, true))
	{
		std::cout << __func__ << "\n";
	}
	Camera::Camera_KeyActions cameraKeyActions(Camera::IDLE);
	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		cameraKeyActions |= Camera::FORWARD;
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		cameraKeyActions |= Camera::BACKWARD;
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		cameraKeyActions |= Camera::LEFT;
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		cameraKeyActions |= Camera::RIGHT;
	if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraKeyActions |= Camera::SPEED_UP;
	camera.processKeyAction(cameraKeyActions);


	camera.update(deltaTime);
	world.Tick(deltaTime);
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
void Demo0_RS::draw(float deltaTime)
{
	static bool first = false;
	if (!std::exchange(first, true))
	{
		std::cout << __func__ << "\n";
	}

	const glm::mat4 view = camera.getViewMatrix();
	const glm::mat4 proj = camera.getProjMatrix();
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
	ImGui::DragFloat3("Light Ratio", &lightRatio.x, 0.01, 0, 10, "%.3f");
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
	glm::vec3 lightWorldPos = rotation * glm::vec4{ 1.2f, 5.2f, 10.0f, 1.0f } *2.0f;


	ImGui::DragFloat("Moving Speed:", &velocity, 1, -1440, 1440);



	// 光源
	Light light = { lightColor * lightRatio.x,lightColor * lightRatio.y,lightColor * lightRatio.z, lightWorldPos };

	ImGui::Text("%s", std::format("Light Position: {:.3f}, {:.3f}, {:.3f}", light.position.x, light.position.y, light.position.z).c_str());

	ImGui::End();
	// 创建发光体
	{
		// 灯光模型的变换矩阵
		glm::mat4 lightWorldMatrix(1.0f);
		// 设置位置
		lightWorldMatrix[3] = { light.position,1 };
		// 先缩放模型大小
		lightWorldMatrix = glm::scale(lightWorldMatrix, glm::vec3(0.2f));
		illuminantModel->setLightColor(light.specular);
		illuminantModel->draw(camera, lightWorldMatrix);
	}
	//随机位置生成盒子
	{
		lightModel2->setLight(light);
		auto wMatsView = boxArguments | std::ranges::views::transform([](auto&& tuple) {
			return glm::scale(glm::translate(glm::mat4(1.0f), std::get<0>(tuple) + glm::vec3{ 0, 0.0001, 0 }), glm::vec3(static_cast<float>(std::get<1>(tuple))));
			});
		lightModel2->draw(camera, { wMatsView.begin(),wMatsView.end() });
	}

	{
		//d->lightModel2->setLight(light);
		//d->lightModel2->draw(data->camera, glm::translate(glm::mat4(1.0f),{0,0,1.0f} ));
	}

	// 地面
	{
		lightModel->setLight(light);
		glm::mat4 wMat(1.0f);
		wMat[3] = { 0.0f,-1.0f,0.0f,wMat[3][3] };
		wMat = glm::scale(wMat, { 150.0f,1.f,150.0f });
		lightModel->setMaterial(MaterialTable::instance()->getMaterial("default"));
		lightModel->draw(camera, wMat);
	}
	{
		static auto shader = ShaderProgram::makeShaderByName("common.vert", "common.frag");
		static Model model(R"(C:\Users\zhang\Pictures\Material\F22\f22.obj)");
		shader->use();
		shader->glUniform("lights[0].type", 1);
		shader->glUniform("viewPos", camera.Position);
		shader->glUniform("lights[0].position", light.position);
		shader->glUniform("lights[0].ambient", light.ambient);
		shader->glUniform("lights[0].diffuse", light.diffuse);
		shader->glUniform("lights[0].specular", light.specular);
		shader->glUniform("lights[0].constant", 1.0f);
		shader->glUniform("lights[0].linear", 0.f);
		shader->glUniform("lights[0].quadratic", 0.f);
		shader->glUniform("material.shininess", 32.0f);
		shader->glUniform("view", camera.getViewMatrix());
		shader->glUniform("projection", camera.getProjMatrix());


		ImGui::Begin("F22");
		static glm::vec3 scale = { 1,1,1 };
		static glm::vec3 position = { 0,0,0 };
		static glm::vec3 axis = {0,1,0};
		static float roll = 0;
		glm::quat qua;

		if (ImGui::DragFloat3("Scale", &scale[0], 0.1f)) {};
		if (ImGui::DragFloat3("Position", &position[0], 0.1f)){};
		if constexpr (false)
		{
			glm::vec3 forward = {
			cos(euler.pitch) * cos(euler.yaw)
			,sin(euler.pitch)
			,cos(euler.pitch) * sin(euler.yaw)
			};

			// crossdot({1,0,0} , forward)
			axis = {
				0,
				-cos(euler.pitch) * sin(euler.yaw),
				sin(euler.pitch)
			};

			float angle = glm::degrees(acos(cos(euler.pitch / 2) * cos(euler.yaw / 2)));
			ImGui::DragFloat3("Forward", &forward[0], 0.1f);
			{
				auto euler_degrees = glm::degrees(euler.data);
				if(ImGui::DragFloat3("Rotation", &euler_degrees[0], 0.1f))
				{
					euler.data = glm::radians(euler_degrees);
				}
			}
			{
				auto l_axis = glm::cross({ 1,0,0 }, forward);
				ImGui::DragFloat3("Axis", &l_axis[0], 0.1f);
			}
			ImGui::DragFloat3("myAxis", &axis[0], 0.1f);
			ImGui::DragFloat("Angle", &angle, 0.1f);

			auto rot_roll = glm::rotate(glm::quat(1, 0, 0, 0), euler.roll, { 0,0,1 });
			auto rot_yaw = glm::rotate(glm::quat(1, 0, 0, 0), euler.yaw, { 0,1,0 });
			auto rot_pitch = glm::rotate(glm::quat(1, 0, 0, 0), euler.pitch, rot_yaw * glm::vec3{ 1,0,0 });
			qua = normalize(rot_pitch * rot_yaw * rot_roll);

				
			//qua = normalize(glm::quat( 1 + cos(euler.pitch) * cos(euler.yaw), axis));

			//qua = glm::quat({ 1,0,0 }, forward);
		}else
		{
			if (ImGui::DragFloat3("Axis", &axis[0], 0.1f))
			{
				
			}
			if (ImGui::DragFloat("Roll", &roll, 0.1f))
			{
				
			}
			qua = glm::quat(cos(glm::radians(roll)), normalize(axis) * sin(glm::radians(roll)));

			qua = glm::angleAxis(glm::radians(roll), axis);
		}

		
		if(ImGui::DragFloat4("Quat", &qua[0], 0.1f))
		{
			
		}
		{
			constexpr glm::vec3 world_up = { 0,1,0 };
			auto rMat = glm::mat3_cast(qua);
			auto& [axisX, axisY, axisZ] = reinterpret_cast<std::array<glm::vec3, 3>&>(rMat[0]);

			const auto forward = -axisZ;
			const auto right = glm::cross(forward, world_up);
			const auto up = glm::normalize(glm::cross(right, forward)) ;


			const float forward_proj_xz = sqrt(forward.x * forward.x + forward.z * forward.z);
			const float pitch = atan2(forward.y, forward_proj_xz);
			const float yaw = -atan2(forward.z, forward.x) - glm::pi<float>() / 2;

			auto rot_yaw = glm::rotate(glm::quat(1, 0, 0, 0), yaw, { 0,1,0 });
			auto rot_pitch = glm::rotate(glm::quat(1, 0, 0, 0), pitch, rot_yaw * glm::vec3{ 1, 0, 0 });
			auto rot_roll =  inverse((rot_pitch * rot_yaw)) * qua;

			//const auto f = glm::cross(up, axisY);
			const auto roll = glm::angle(normalize(rot_roll));//acos(glm::clamp(dot(up, axisY), -1.0f, 1.0f));


			glm::vec3 rot = glm::degrees(glm::vec3{ pitch , yaw, roll});

			ImGui::DragFloat3("Axis->Rotation", &rot[0], 0.1f);

			auto axis = glm::axis(qua);
			ImGui::DragFloat3("Quat->Axis", &axis[0], 0.1f);

			auto q = glm::angleAxis(roll,axis);
			ImGui::DragFloat4("Quat->Quat", &q[0], 0.1f);

			//qua = q;
		}
		ImGui::End();


		shader->glUniform("model", glm::translate(glm::mat4(1.0f), position) 
			* glm::mat4_cast(qua) 
			* glm::rotate(glm::mat4(1),glm::pi<float>() , {0,1,0}) 
			* glm::scale(glm::mat4(1.0f), scale));
		model.Draw(*shader);
	}



	
}

void Demo0_RS::resizeEvent(int width, int height)
{
	GUIInterface::resizeEvent(width, height);
	if (width > 0 && height > 0)
	{
		camera.resizeViewport(width, height);
	}
}

void Demo0_RS::keyEvent(int keyCode, int scanCode, int keyAction, int keyModifiers)
{
	GUIInterface::keyEvent(keyCode, scanCode, keyAction, keyModifiers);
	input->keyEvent(keyCode, keyAction, keyModifiers);
}

void Demo0_RS::mouseButtonEvent(int buttonCode, int keyAction, int keyModifiers)
{
	GUIInterface::mouseButtonEvent(buttonCode, keyAction, keyModifiers);
	input->mouseButtonEvent(buttonCode, keyAction, keyModifiers);
}

void Demo0_RS::mouseMoveEvent(float mouseX, float mouseY, float deltaX, float deltaY)
{
	GUIInterface::mouseMoveEvent(mouseX, mouseY, deltaX, deltaY);
	input->mouseMoveEvent(deltaX, deltaY);
	camera.processMouseMove(deltaX, -deltaY);
}

void Demo0_RS::scrollEvent(float deltaX, float deltaY)
{
	GUIInterface::scrollEvent(deltaX, deltaY);
	input->scrollEvent(deltaX, deltaY);
	camera.processMouseScroll(deltaY);
}
