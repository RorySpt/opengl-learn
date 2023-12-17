#pragma once
#include "actor.h"
#include "actor_input_component.h"
#include "camera_manager.h"


class PlayerController:public Actor
{
	ClassMetaDeclare(PlayerController)
public:
	PlayerController();

	void OnConstruct() override;
	void OnProcess(Actor* actor);
	void OnUnPossess();
	void BeginPlay() override;

	void Tick(float deltaTime) override;
	// test
	void SetupPlayerInputComponent(InputComponent* input_component) override;
	void OnMoveForward(float value) const;

	InputManager* GetInputManager() const;
	CameraManager* GetCameraManager();

	Actor* _processActor = nullptr;
	InputComponent* _input_component = nullptr;
	CameraManager* _camera_manager = nullptr;
};

