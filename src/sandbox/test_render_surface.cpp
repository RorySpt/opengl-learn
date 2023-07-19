#include "stdafx.h"
#include "test_render_surface.h"

#include "Model.h"
#include "player_controller.h"


TestRenderSurface::~TestRenderSurface() = default;

TestRenderSurface::TestRenderSurface()
	:mt(std::random_device()())
{

}

void TestRenderSurface::init(GLFWwindow* window)
{
	_window = window;
	std::cout << __func__ << "\n";

}

void TestRenderSurface::exit()
{
	std::cout << __func__ << "\n";
	world.EndPlay();
}

void TestRenderSurface::tick(float deltaTime)
{

}

void TestRenderSurface::draw(float deltaTime)
{
	
}

void TestRenderSurface::resizeEvent(int width, int height)
{
	GUIInterface::resizeEvent(width, height);
	
}

void TestRenderSurface::keyEvent(int keyCode, int scanCode, int keyAction, int keyModifiers)
{
	GUIInterface::keyEvent(keyCode, scanCode, keyAction, keyModifiers);
	
}

void TestRenderSurface::mouseButtonEvent(int buttonCode, int keyAction, int keyModifiers)
{
	GUIInterface::mouseButtonEvent(buttonCode, keyAction, keyModifiers);
	
}

void TestRenderSurface::mouseMoveEvent(float mouseX, float mouseY, float deltaX, float deltaY)
{
	GUIInterface::mouseMoveEvent(mouseX, mouseY, deltaX, deltaY);
	
}

void TestRenderSurface::scrollEvent(float deltaX, float deltaY)
{
	GUIInterface::scrollEvent(deltaX, deltaY);
}
