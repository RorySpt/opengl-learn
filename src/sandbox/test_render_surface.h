#pragma once
#include "BoxModel.h"
#include "camera.h"
#include "igui.h"
#include "IlluminantModel.h"
#include "input_manager.h"
#include "world.h"

class TextureLoader;
class TestRenderSurface final:
    public GUIInterface
{
	CLASS_NO_COPY_AND_MOVE(TestRenderSurface)
public:
	
	~TestRenderSurface() override;


	TestRenderSurface();

public:
	void init(GLFWwindow*) override;
	void exit() override;
	void tick(float deltaTime) override;
	void draw(float deltaTime) override;
	
	void resizeEvent(int width, int height) override;
	void keyEvent(int keyCode, int scanCode, int keyAction, int keyModifiers) override;
	void mouseButtonEvent(int buttonCode, int keyAction, int keyModifiers) override;
	void mouseMoveEvent(float mouseX, float mouseY, float deltaX, float deltaY) override;
	void scrollEvent(float deltaX, float deltaY) override;

private:
	void UI_CameraTest();
	void UI_Scene();

	// “¿¿µ_window
	void InitWorld();

	GLFWwindow* _window = nullptr;


	Camera camera;
	World world;
	InputManager* input = nullptr;

	std::shared_ptr<TextureLoader> texture_loader;
	std::default_random_engine mt;
};

