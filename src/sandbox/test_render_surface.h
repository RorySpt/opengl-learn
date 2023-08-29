#pragma once
#include "BoxModel.h"
#include "camera.h"
#include "igui.h"
#include "IlluminantModel.h"
#include "input_manager.h"
#include "world.h"

class CameraManager;
class TextureLoader;
class TestRenderSurface final:
    public GUIInterface
{
	CLASS_NO_COPY_AND_MOVE(TestRenderSurface)
public:
	
	~TestRenderSurface() override;


	TestRenderSurface();


	void init(GLFWwindow*) override;
	void exit() override;
	void tick(float deltaTime) override;
	void draw(float deltaTime) override;

private:
	void UI_CameraTest();
	void UI_Scene();

	// “¿¿µ_window
	void InitWorld();

	GLFWwindow* _window = nullptr;


	World world;
	InputManager* input = nullptr;
	CameraManager* camera_manager = nullptr;
	std::shared_ptr<TextureLoader> texture_loader;
	std::default_random_engine mt;
};

