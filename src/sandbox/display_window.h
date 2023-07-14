#pragma once
#include "BoxModel.h"
#include "common.h"
#include "IlluminantModel.h"
#include "material.h"

struct CGLWindowData;
class CGLWindowPrivate;

struct CGLWindowData
{
	std::shared_ptr<BoxModel> boxModel;
	//光源模型
	std::shared_ptr<LightBoxModel> lightModel;
	std::shared_ptr<LightBoxModel2> lightModel2;
	std::shared_ptr<IlluminantModel> illuminantModel;

	Camera camera;

	std::vector<std::tuple<glm::vec3, double, Material>> boxArguments;
};

class CGLWindow 
{
	Q_DECLARE_PRIVATE(CGLWindow)
	CLASS_NO_COPY_AND_MOVE(CGLWindow)

	using Clock = std::chrono::high_resolution_clock;
	using Duration = std::chrono::duration<float>;
	using TimePoint = Clock::time_point; 
	
public:
	enum class MouseMode
	{
		Enabled,
		Disabled
	};
	CGLWindow();
	virtual ~CGLWindow();


	int exec();
protected:
	// These will invoke by exec.
	void render_init();
	void render_exit();
	void render_draw(float deltaTime); // specific invoke by processDraw

	void processFrameBegin(float deltaTime);
	void processFrameEnd(float deltaTime);
	void processInput(float deltaTime);
	void processUpdate(float deltaTime);
	void processDraw(float deltaTime);
	

	// These will invoke by glfw event.
	void on_resize(int width, int height);
	void on_key(int key, int scan_code, int action, int mods);
	void on_mouseMove(float x_pos, float y_pos, float x_offset, float y_offset);
	void on_scroll(float x_offset, float y_offset);
private:
	std::unique_ptr<CGLWindowPrivate> d_ptr;
	std::unique_ptr<CGLWindowData> data;
};

