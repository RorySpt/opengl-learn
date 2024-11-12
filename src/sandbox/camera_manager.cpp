#include "camera_manager.h"
#include "camera.h"
#include "IlluminationWidget.h"
#include "window_manager.h"
CameraManager::CameraManager()
{
	defaultCamera = std::make_shared<Camera>();
	RegisterCamera(defaultCamera);
}

CameraManager::~CameraManager()
{
	const auto evt_d = WindowManagerInstance->GetEventDispatcher(window);
	evt_d->resizeHandler.unbind(handle);
}

void CameraManager::init(GLFWwindow* w)
{
	if (window != nullptr) return; // 不接受重复初始化
	this->window = w;
	const auto evt_d = WindowManagerInstance->GetEventDispatcher(w);
	handle = evt_d->resizeHandler.bind([&](const Event<EventType::Resize>& e)
		{
			OnResizeViewport(e.getWidth(), e.getHeight());
		});
}

std::weak_ptr<Camera> CameraManager::RequestCamera()
{
	auto camera = std::make_shared<Camera>();
	RegisterCamera(camera);
	return camera;
}

void CameraManager::RegisterCamera(const std::shared_ptr<Camera>& camera)
{
	if (camera)
		cameras.insert(camera);
}

void CameraManager::RemoveCamera(const std::weak_ptr<Camera>& camera)
{
	if (!camera.expired())
		cameras.erase(camera.lock());
}

void CameraManager::Activate(const std::weak_ptr<Camera>& camera)
{
	if (camera.expired())
		return;

	if (cameras.contains(camera.lock()))
	{
		activatedCamera = camera.lock();
	}
}

std::weak_ptr<Camera> CameraManager::ActivatedCamera()
{
	return activatedCamera ? activatedCamera : defaultCamera;
}

void CameraManager::ResetActivate()
{
	activatedCamera = defaultCamera;
}

void CameraManager::OnResizeViewport(int w, int h) const
{
	for (auto& camera : cameras)
	{
		camera->resizeViewport(w, h);
	}
}
