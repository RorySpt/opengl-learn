#pragma once
#include "actor.h"
#include "actor_input_component.h"
class PlayerController:public Actor
{
	ClassMetaDeclare(PlayerController)
public:
	PlayerController();

	void OnProcess(Actor* actor);
	void OnUnPossess();
	void BeginPlay() override;

	// test
	void SetupPlayerInputComponent(InputComponent* input_component) override;
	void OnMoveForward(float value) const;

	InputManager* GetInputManager() const;

	Actor* _processActor = nullptr;
	InputComponent* _input_component;
};

