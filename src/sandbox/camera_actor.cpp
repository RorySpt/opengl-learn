#include "camera_actor.h"

#include "actor_camera_component.h"
#include "actor_input_component.h"
#include "player_controller.h"
#include "actor_light_component.h"

DEF_InputAction(Run, EKeyCode::K_LeftShift);

CameraActor::CameraActor()
{
	_camera_component = CreateDefaultComponent<CameraComponent>();
	_camera_component->AttachToComponent(_root_component);

	_spot_light_component = CreateDefaultComponent<SpotLightComponent>();
	_spot_light_component->AttachToComponent(_root_component);
	_spot_light_component->b_show_model = false;
	_spot_light_component->lightColor = glm::vec3{0};
}

void CameraActor::BeginPlay()
{
	Actor::BeginPlay();
	GetWorld()->_playerController->OnProcess(this);
}

void CameraActor::SetupPlayerInputComponent(InputComponent* input_component)
{
	Actor::SetupPlayerInputComponent(input_component);

	input_component->BindAction("Run", EKeyAction::A_Press, this, [&]()
		{
			run = true;
		});
	input_component->BindAction("Run", EKeyAction::A_Release, this, [&]()
		{
			run = false;
		});
	input_component->BindAxis("MoveForward", this, [&](float val)
		{
			_root_component->SetRelativeLocation(_root_component->GetRelativeLocation() + _root_component->GetRelativeRotation() * glm::vec3{0, 0, -val} * moveSpeed * (run ? 5.f:1.f));
		});
	input_component->BindAxis("MoveBack", this, [&](float val)
		{
			_root_component->SetRelativeLocation(_root_component->GetRelativeLocation() + _root_component->GetRelativeRotation() * glm::vec3{0, 0, val} * moveSpeed* (run ? 5.f : 1.f));
		});
	input_component->BindAxis("MoveLeft", this, [&](float val)
		{
			_root_component->SetRelativeLocation(_root_component->GetRelativeLocation() + _root_component->GetRelativeRotation() * glm::vec3{-val, 0, 0} * moveSpeed* (run ? 5.f : 1.f));
		});
	input_component->BindAxis("MoveRight", this, [&](float val)
		{
			_root_component->SetRelativeLocation(_root_component->GetRelativeLocation() + _root_component->GetRelativeRotation() * glm::vec3{val, 0, 0} * moveSpeed* (run ? 5.f : 1.f));
		});

	input_component->BindAxis("LookAround", this, [&](glm::vec2 val)
		{
			
			glm::vec3 euler = glm::degrees(convertToEulerAngle(_root_component->GetRelativeRotation()).data);

			int width, height;
			glfwGetWindowSize(GetWorld()->GetPlayerController()->GetInputManager()->GetWindow(), &width, &height);

			const float Sensitivity = 0.1f;
			const auto scaleW = static_cast<float>(30.0f / width);
			const auto scaleH = static_cast<float>(30.0f / height);

			euler.y += -val.x * Sensitivity;
			euler.x =  glm::clamp(euler.x + -val.y * Sensitivity, -89.99f, 89.99f);



			_root_component->SetRelativeRotation(convertToQuaternion(glm::radians(euler)));

		});

}
