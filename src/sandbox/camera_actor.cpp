#include "camera_actor.h"

#include "actor_camera_component.h"
#include "actor_input_component.h"
#include "player_controller.h"

CameraActor::CameraActor()
{
	_camera_component = CreateDefaultComponent<CameraComponent>();
	_camera_component->AttachToComponent(_root_component);
}

void CameraActor::BeginPlay()
{
	Actor::BeginPlay();
	GetWorld()->_playerController->OnProcess(this);
}

void CameraActor::SetupPlayerInputComponent(InputComponent* input_component)
{
	Actor::SetupPlayerInputComponent(input_component);
	input_component->BindAxis("MoveForward", this, [&](float val)
		{
			if (val != 0) _root_component->SetRelativeLocation(_root_component->GetRelativeLocation() + _root_component->GetRelativeRotation() * glm::vec3{0, 0, -val});
		});
	input_component->BindAxis("MoveBack", this, [&](float val)
		{
			if (val != 0) _root_component->SetRelativeLocation(_root_component->GetRelativeLocation() + _root_component->GetRelativeRotation() * glm::vec3{0, 0, val});
		});
	input_component->BindAxis("MoveLeft", this, [&](float val)
		{
			if (val != 0) _root_component->SetRelativeLocation(_root_component->GetRelativeLocation() + _root_component->GetRelativeRotation() * glm::vec3{-val, 0, 0});
		});
	input_component->BindAxis("MoveRight", this, [&](float val)
		{
			if (val != 0) _root_component->SetRelativeLocation(_root_component->GetRelativeLocation() + _root_component->GetRelativeRotation() * glm::vec3{val, 0, 0});
		});

	input_component->BindAxis("LookAround", this, [&](glm::vec2 val)
		{
			if (val.x != 0 || val.y != 0)
			{
				EulerAngle euler = glm::degrees(convertToEulerAngle(_root_component->GetRelativeRotation()).data);

				int width, height;
				glfwGetWindowSize(GetWorld()->GetPlayerController()->GetInputManager()->GetWindow(), &width, &height);
				const auto scaleW = static_cast<float>(360.0 / width);
				const auto scaleH = static_cast<float>(360.0 / height);

				euler.yaw += val.x * scaleW;
				euler.pitch = glm::clamp(euler.pitch + val.y * scaleH, -90.f, 90.f);

				_root_component->SetRelativeRotation(convertToQuaternion(glm::radians(euler.data)));
			}

		});

}
