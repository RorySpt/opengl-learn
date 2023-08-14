#pragma once
#include "actor_scene_component.h"


class Camera;
class CameraManager;

//#include "camera.h"
class CameraComponent :
    public SceneComponent
{
	ClassMetaDeclare(CameraComponent)
public:
	void BeginPlay() override;
	void TickComponent(float deltaTime) override;


	std::weak_ptr<Camera> camera_weak_ptr;

	CameraManager* camera_manager;
};

