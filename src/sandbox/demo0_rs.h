#pragma once
#include <random>

#include "BoxModel.h"
#include "camera.h"
#include "igui.h"
#include "IlluminantModel.h"
#include "input_manager.h"
#include "world.h"



class Demo0_RS final :
    public GUIInterface
{
public:
	Demo0_RS();
	~Demo0_RS() override;
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
	GLFWwindow* _window;



	//受光源影响的光源模型
	std::shared_ptr<BoxModel_SingleColor> lightModel;
	std::shared_ptr<BoxModel_SimpleTexture> lightModel2;
	std::shared_ptr<IlluminantModel> illuminantModel;

	Camera camera;
	World world;
	InputManager* input;
	std::vector<std::tuple<glm::vec3, double, Material>> boxArguments;


	std::default_random_engine mt;
};

