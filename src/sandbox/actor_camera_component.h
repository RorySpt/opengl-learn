#pragma once
#include "actor_scene_component.h"
//#include "camera.h"
class CameraComponent :
    public SceneComponent
{
	ClassMetaDeclare(CameraComponent)
public:
	void TickComponent(float deltaTime) override;

};

