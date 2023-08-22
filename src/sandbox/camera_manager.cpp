#include "camera_manager.h"
#include "camera.h"
CameraManager::CameraManager()
{
	defaultCamera = std::make_shared<Camera>();
	RegisterCamera(defaultCamera);
}

std::weak_ptr<Camera> CameraManager::RequestCamera()
{
	auto camera = std::make_shared<Camera>();
	RegisterCamera(camera);
	return camera;
}

void CameraManager::RegisterCamera(const std::shared_ptr<Camera>& camera)
{
	if(camera)
		cameras.insert(camera);
}

void CameraManager::RemoveCamera(const std::weak_ptr<Camera>& camera)
{
	if(!camera.expired())
		cameras.erase(camera.lock());
}

void CameraManager::Activate(const std::weak_ptr<Camera>& camera)
{
	if(camera.expired())
		return;

	if(cameras.contains(camera.lock()))
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

void CameraManager::ResizeViewport(int w, int h)
{
	for(auto& camera:cameras)
	{
		camera->resizeViewport(w, h);
	}
}
