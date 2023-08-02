#include "actor_camera_component.h"

#include <format>
#include <imgui.h>
#include <iostream>

void CameraComponent::TickComponent(float deltaTime)
{
	SceneComponent::TickComponent(deltaTime);

	auto _loc_world = GetComponentToWorld();

	//std::cout << std::format("Positon:{},{},{}\n", _loc_world[3][0], _loc_world[3][1], _loc_world[3][2]);

	
}


