#pragma once
#include "actor.h"
class CameraComponent;

class CameraActor :
    public Actor
{
	ClassMetaDeclare(CameraActor)
public:
	CameraActor();
	void BeginPlay() override;
	void SetupPlayerInputComponent(InputComponent* input_component) override;

	CameraComponent* _camera_component;
};

