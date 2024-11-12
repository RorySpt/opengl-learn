#include "actor_camera_component.h"

#include <format>
#include <imgui.h>
#include <iostream>

#include "actor.h"
#include "camera.h"
#include "player_controller.h"

void CameraComponent::BeginPlay()
{
	SceneComponent::BeginPlay();

	camera_manager = GetOwner()->GetWorld()->GetPlayerController()->GetCameraManager();

	camera_weak_ptr = camera_manager->RequestCamera();

	const auto camera = camera_weak_ptr.lock();
	glfwGetWindowSize(camera_manager->window, &camera->VWidth, &camera->VHeight);
}

void CameraComponent::TickComponent(float deltaTime)
{
	SceneComponent::TickComponent(deltaTime);

	auto _loc_world = GetComponentToWorld();

	//std::cout << std::format("Positon:{},{},{}\n", _loc_world[3][0], _loc_world[3][1], _loc_world[3][2]);
	auto camera = camera_weak_ptr.lock();

	camera->Position = GetWorldLocation();

	auto euler = convertToEulerAngle(GetWorldRotation());

	camera->Pitch = glm::degrees(euler.pitch);
	camera->Yaw = glm::degrees(euler.yaw);
}

void CameraComponent::UI_Draw()
{
	SceneComponent::UI_Draw();
	auto camera = camera_weak_ptr.lock();
	if(camera)
	{
		camera->drawUI();
	}
}


