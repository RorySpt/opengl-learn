#pragma once
#include "actor.h"
#include "event_dispatcher.h"

class GLFWwindow;
class Camera;

class CameraManager :public Actor
{
	ClassMetaDeclare(CameraManager)
public:
	CameraManager();
	~CameraManager();
	
	void init(GLFWwindow*);
	


	std::weak_ptr<Camera> RequestCamera();

	void RegisterCamera(const std::shared_ptr<Camera>&);
	void RemoveCamera(const std::weak_ptr<Camera>&);

	void Activate(const std::weak_ptr<Camera>&);

	std::weak_ptr<Camera> ActivatedCamera();

	void ResetActivate();

	void OnResizeViewport(int w, int h) const;

	std::shared_ptr<Camera> activatedCamera;
	std::shared_ptr<Camera> defaultCamera;

	std::set<std::shared_ptr<Camera>> cameras;

	GLFWwindow* window = nullptr;

	EventDispatcher::ResizeHandler::handle handle;
};

