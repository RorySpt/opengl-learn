#pragma once
#include "BoxModel.h"
#include "common.h"
#include "IlluminantModel.h"
#include "material.h"

// window just window, The rest of the logic is stripped out

struct DisplayWindowData;
class DisplayWindowPrivate;

struct DisplayWindowData
{
	std::shared_ptr<BoxModel> boxModel;
	//光源模型
	std::shared_ptr<LightBoxModel> lightModel;
	std::shared_ptr<LightBoxModel2> lightModel2;
	std::shared_ptr<IlluminantModel> illuminantModel;

	Camera camera;

	std::vector<std::tuple<glm::vec3, double, Material>> boxArguments;
};

class DisplayWindow 
{
	Q_DECLARE_PRIVATE(DisplayWindow)
	CLASS_NO_COPY_AND_MOVE(DisplayWindow)

	using Clock = std::chrono::high_resolution_clock;
	using Duration = std::chrono::duration<float>;
	using TimePoint = Clock::time_point; 
	
public:
	enum class MouseMode
	{
		Enabled,
		Disabled
	};
	DisplayWindow();
	virtual ~DisplayWindow();


	int exec();
protected:
	// These will invoke by exec.
	void render_init();
	void render_exit();
	void render_tick(float deltaTime);

	void render_draw(float deltaTime); // specific invoke by processDraw

	void processFrameBegin(float deltaTime);
	void processFrameEnd(float deltaTime);
	void processInput(float deltaTime);
	void processUpdate(float deltaTime);
	void processDraw(float deltaTime);
	

	// These will invoke by glfw event.
	void resizeEvent(int width, int height);
	void keyEvent(int keyCode, int scanCode, int keyAction, int keyModifiers);
	void mouseMoveEvent(float mouseX, float mouseY, float deltaX, float deltaY);
	void scrollEvent(float deltaX, float deltaY);


	void toggleFullscreenMode();
	void toggleMouseMode();

	GLFWmonitor* findMonitorByPosition(int centerX, int centerY);
	
private:
	std::unique_ptr<DisplayWindowPrivate> d_ptr;
	std::unique_ptr<DisplayWindowData> data;
};

