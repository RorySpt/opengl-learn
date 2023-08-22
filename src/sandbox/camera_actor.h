#pragma once
#include "actor.h"
class CameraComponent;

class SpotLightComponent;
class CameraActor :
    public Actor
{
	ClassMetaDeclare(CameraActor)
public:
	CameraActor();
	void BeginPlay() override;
	void SetupPlayerInputComponent(InputComponent* input_component) override;

	CameraComponent* _camera_component;
	SpotLightComponent* _spot_light_component;

	float moveSpeed = 5;
	bool run = false;
};

