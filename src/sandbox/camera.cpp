#include "stdafx.h"
#include "camera.h"

#include <imgui.h>


void Camera::updateCameraByKeyAction(float deltaTime)
{
	const Camera_KeyActions actions = std::exchange(FrameKeyAction, IDLE);


	//MovementSpeedRatio = actions.testFlag(SPEED_UP) ? CAMERA_DEFAULT_SPEED_ACCELERATION : -CAMERA_DEFAULT_SPEED_DECELERATION;
	ImGui::Begin("Debug");
	MovementSpeed = actions.testFlag(SPEED_UP) ? MovementSpeedMax : MovementSpeedMin;

	//MovementSpeedRatio = actions.testFlag(SPEED_UP) ? 1.0f : 1.0f;
	//MovementSpeed = glm::clamp(MovementSpeed, CAMERA_DEFAULT_SPEED_MIN, CAMERA_DEFAULT_SPEED_MAX);
	const float velocity = MovementSpeed * MovementSpeedRatio;
	const glm::vec2 direction = convertKeyActionsToVec2(actions);
	//comm::print("direction: {}, {}\n", direction.x,direction.y);

	//std::string_view s("direction: {}, {}");
	//std::cout << std::format(std::string_view("direction: {}, {}"), direction.x, direction.y) << std::endl;
	if (actions & 0X0F)
	{
		const glm::vec3 moveDirection = normalize((Front * direction.y + Right * direction.x));
		Position += moveDirection * velocity * deltaTime;
	}

	ImGui::Text("%s", std::format("direction: {}, {}", direction.x, direction.y).c_str());
	ImGui::End();
}

void Camera::drawDebugInfo()
{
	ImGui::Begin("Camera", &bShowDebugWindow);

	
	drawUI();

	ImGui::End();
}

void Camera::drawUI()
{
	Camera& camera = *this;
	ImGui::Text("camera current speed: %f", MovementSpeed * MovementSpeedRatio);
	ImGui::DragFloat3("camera current position", &Position[0], 0.1);

	ImGui::SliderFloat("camera MovementSpeedRatio", &camera.MovementSpeedRatio, 1.0, 10);
	ImGui::SliderFloat("camera MovementSpeedMax", &camera.MovementSpeedMax, camera.MovementSpeedMin, 100);
	ImGui::SliderFloat("camera MovementNormalSpeed", &camera.MovementSpeedMin, 0, 100);

	ImGui::SliderFloat("camera ZFar", &camera.ZFar, camera.ZNear, 1000);
	ImGui::SliderFloat("camera ZNear", &camera.ZNear, 0, 100);

	ImGui::SliderFloat("camera Zoom", &camera.Zoom, 1, 360);

	ImGui::SliderFloat("camera pitch", &camera.Pitch, -89, 89);
	auto inverseYaw = -camera.Yaw;
	ImGui::SliderFloat("camera -yaw", &inverseYaw, -180, 180);
	camera.Yaw = -inverseYaw;

	ImGui::Text("camera right-vector: % .5f, % .5f, % .5f", camera.Right.x, camera.Right.y, camera.Right.z);
	ImGui::Text("camera up-vector: % .5f, % .5f, % .5f", camera.Up.x, camera.Up.y, camera.Up.z);
	ImGui::Text("camera front-vector: % .5f, % .5f, % .5f", camera.Front.x, camera.Front.y, camera.Front.z);

}
