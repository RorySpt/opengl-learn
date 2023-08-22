#pragma once
#include "actor.h"


class Camera;

class CameraManager :public Actor
{
	ClassMetaDeclare(CameraManager)
public:
	CameraManager();

	std::weak_ptr<Camera> RequestCamera();

	void RegisterCamera(const std::shared_ptr<Camera>&);
	void RemoveCamera(const std::weak_ptr<Camera>&);

	void Activate(const std::weak_ptr<Camera>&);

	std::weak_ptr<Camera> ActivatedCamera();

	void ResetActivate();

	void ResizeViewport(int w, int h);

	std::shared_ptr<Camera> activatedCamera;
	std::shared_ptr<Camera> defaultCamera;

	std::set<std::shared_ptr<Camera>> cameras;
};

