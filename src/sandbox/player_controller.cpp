#include "player_controller.h"

#include "common.h"

PlayerController::PlayerController()
{
	_input_component = new InputComponent;
	_components.emplace_back(_input_component);

	

}

void PlayerController::OnProcess(Actor* actor)
{
	if(GetWorld()->Within(actor))
	{
		_input_component->RemoveBindings();
		_processActor = actor;
		actor->SetupPlayerInputComponent(_input_component);
	}
}

void PlayerController::OnUnPossess()
{
	_input_component->RemoveBindings();
	_processActor = nullptr;
}

void PlayerController::BeginPlay()
{
	Actor::BeginPlay();

	OnProcess(this);
}

void PlayerController::SetupPlayerInputComponent(InputComponent* input_component)
{
	Actor::SetupPlayerInputComponent(input_component);
	input_component->BindAction("Jump", EKeyAction::A_Press, this, []()
		{
			std::cout << comm::GetCurrentTimeString() <<" PlayerController:Jump!!\n";
		});

	static float distance = 0;
	input_component->BindAxis("MoveForward", this, [](float value)
		{
			if(value>0)
			{
				std::cout << std::format("{} PlayerController:MoveForward: {}, Total: {}\n"
					, comm::GetCurrentTimeString(), value, (distance += value));
			}
		});
}
