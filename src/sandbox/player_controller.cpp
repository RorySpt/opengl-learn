#include "player_controller.h"

#include "actor_camera_component.h"
#include "common.h"
#include "camera.h"

PlayerController::PlayerController()
{
	_input_component = CreateDefaultComponent<InputComponent>();
}

void PlayerController::OnConstruct()
{
	Actor::OnConstruct();
}

//auto findCameraComponent(Actor* actor)
//{
//	for(auto& com: actor)
//	{
//		com->
//	}
//}

void PlayerController::OnProcess(Actor* actor)
{
	if(GetWorld()->Within(actor))
	{
		_input_component->RemoveBindings();
		_processActor = actor;
		actor->SetupPlayerInputComponent(_input_component);
		if(actor->bAutoActiveCamera)
		{
			const auto cameraComponents = actor->GetComponentsByType<CameraComponent>();
			if(!cameraComponents.empty())
			{
				_camera_manager->Activate(cameraComponents[0]->camera_weak_ptr);
			}
		}
	}
}

void PlayerController::OnUnPossess()
{
	_camera_manager->ResetActivate();
	_input_component->RemoveBindings();
	_processActor = nullptr;
}

void PlayerController::BeginPlay()
{
	Actor::BeginPlay();
	
	OnProcess(this);
}

void PlayerController::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void PlayerController::SetupPlayerInputComponent(InputComponent* input_component)
{
	Actor::SetupPlayerInputComponent(input_component);
	input_component->BindAction("Jump", EKeyAction::A_Press, this, []()
		{
			std::cout << comm::GetCurrentTimeString() <<" PlayerController:Jump!!\n";
		});

	//static float distance = 0;
	//input_component->BindAxis("MoveForward", this, [](float value)
	//	{
	//		if(value>0)
	//		{
	//			std::cout << std::format("{} PlayerController:MoveForward: {}, Total: {}\n"
	//				, comm::GetCurrentTimeString(), value, (distance += value));
	//		}
	//	});
	input_component->BindAxis("MoveForward", this, &PlayerController::OnMoveForward);
	//std::invoke(&PlayerController::OnMoveForward, dynamic_cast<Actor*>( this), 0);

	//auto mmf = std::mem_fn(&OnMoveForward);
	//auto func = std::bind(OnMoveForward, this, std::placeholders::_1);
	//func(0);
	//auto func = &PlayerController::OnMoveForward;
	//(this->*func)(0);
}

void PlayerController::OnMoveForward(float value) const
{
	//static float distance = 0;
	//if (value > 0)
	//{
	//	std::cout << std::format("{} PlayerController:MoveForward: {}, Total: {}\n"
	//		, comm::GetCurrentTimeString(), value, (distance += value));
	//}
}

InputManager* PlayerController::GetInputManager() const
{
	return &_input_component->inputManager;
}

CameraManager* PlayerController::GetCameraManager() 
{
	if (_camera_manager == nullptr)
	{
		_camera_manager = GetWorld()->SpawnActor<CameraManager>();
	}
		
	return _camera_manager;
}


//void test()
//{
//	void (PlayerController::*func)(float value) = &PlayerController::OnMoveForward;
//	PlayerController controller;
//	std::function<void(float)> f = std::bind_front(std::mem_fn(&PlayerController::OnMoveForward),&controller) ;
//	f(0);
//	//_Is_memfunptr<decltype(func)>::_Class_type
//
//}