// ReSharper disable CppInconsistentNaming
#include "stdafx.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "display_window_private.h"
#include "common.h"
#include "shader.h"
#include "vertices.h"
#include "camera.h"
#include "frame_rate_calc.h"


#include "Model.h"
#include "window_manager.h"


bool WindowRect::Contains(int x, int y) const
{
	return x >= left && x < left + width && y >= top && y < top + height;
}

void DisplayWindowPrivate::initializeOpenGL()
{


	if (!openglInitialized)
	{
		glfwSetErrorCallback(handleErrorEvent);
		//初始化GLF
		if (!(::glfwInit()))
		{
			throw(std::runtime_error(std::format("{} call initializeOpenGL() failed! ", __FUNCTION__)));
		}
		//配置GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//设置主版本号为3
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);//设置次版本号为3
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用核心模式


		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		openglInitialized = true;
	}
}

void DisplayWindowPrivate::initGLFWWindow()
{
	initializeOpenGL();
	Q_Q(DisplayWindow);
	comm::const_cast_ref(window) = createGLFWWindow();
	if (window != nullptr)
	{
		WindowManagerInstance->_windowMap.emplace(window, this);
	}
		

}

GLFWwindow* DisplayWindowPrivate::createGLFWWindow()
{
	Q_Q(DisplayWindow);
	//创建一个窗口对象
	//comm::const_cast_ref(d->window)
	GLFWwindow* window;
	comm::const_cast_ref(window) = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	// 如果glad还没有加载opengl函数，执行加载，整个程序生命周期只会执行一次
	if (!gladLoaded)
	{
		glfwMakeContextCurrent(window);
		//GLAD是用来管理OpenGL的函数指针的，所以在调用任何OpenGL的函数之前初始化GLAD。
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			throw(std::runtime_error(std::format("{} Failed to initialize GLAD", __FUNCTION__)));
		}
		gladLoaded = true;
	}

	glfwMakeContextCurrent(window);
	//设置opengl视口大小，跟实际窗口对应
	glViewport(0, 0, 1280, 720);

	//对窗口注册回调函数(Callback Function)
	glfwSetWindowCloseCallback(window, handleCloseEvent);
	glfwSetKeyCallback(window, handleKeyEvent);
	glfwSetFramebufferSizeCallback(window, handleFrameBufferResizeEvent);
	glfwSetCursorPosCallback(window, handleMouseMoveEvent);
	glfwSetScrollCallback(window, handleScrollEvent);
	glfwSetMouseButtonCallback(window, handleMouseButtonEvent);

	glfwFocusWindow(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	return window;
}

void DisplayWindowPrivate::initializeImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	io.Fonts->AddFontDefault();
	{
		ImFontConfig config;
		config.MergeMode = true;
		io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/simsun.ttc", 13
			, &config, io.Fonts->GetGlyphRangesChineseFull()
		);
		io.Fonts->Build();
	}
	
	for (int i = 18; i < 19; ++i)
	{
		
		ImFontConfig config;


		static const ImWchar basic_ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0,
		};
		static const ImWchar chinese_ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x2000, 0x206F, // General Punctuation
			0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
			0x31F0, 0x31FF, // Katakana Phonetic Extensions
			0xFF00, 0xFFEF, // Half-width characters
			0xFFFD, 0xFFFD, // Invalid
			0x4e00, 0x9FAF, // CJK Ideograms
			0,
		};
		
		ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/CascadiaCode.ttf", static_cast<float>(i)
			, &config, basic_ranges
		);
		
		
		config.MergeMode = true;
		io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/simhei.ttf", static_cast<float>(i)
			, &config, chinese_ranges
		);
		
		
		io.FontDefault = font;
	}
	io.Fonts->Build();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	const char* glsl_version = "#version 130";
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void DisplayWindowPrivate::defaultImGuiDraw()
{
	Q_Q(DisplayWindow);

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader("Windows"))
		{
			const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字
			const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台
			const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号

			ImGui::BulletText("Video Card: %s", biaoshifu);
			ImGui::BulletText("OpenGL Version: %s", OpenGLVersion);

			int MAX_TEXTURE_SIZE;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MAX_TEXTURE_SIZE);
			ImGui::BulletText("GL_MAX_TEXTURE_SIZE: %d", MAX_TEXTURE_SIZE);

			int MAX_ARRAY_TEXTURE_LAYERS;
			glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &MAX_ARRAY_TEXTURE_LAYERS);
			ImGui::BulletText("GL_MAX_ARRAY_TEXTURE_LAYERS: %d", MAX_ARRAY_TEXTURE_LAYERS);

			int MAX_3D_TEXTURE_SIZE;
			glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &MAX_3D_TEXTURE_SIZE);
			ImGui::BulletText("GL_MAX_3D_TEXTURE_SIZE: %d", MAX_3D_TEXTURE_SIZE);
			

			int left, top, width, height;

			glfwGetWindowPos(window, &left, &top);
			glfwGetWindowSize(window, &width, &height);
			const int centerX = left + width / 2;
			const int centerY = top + height / 2;
			// 找到中心点所在窗口
			GLFWmonitor* workMonitor = q->findMonitorByPosition(centerX, centerY);

			if (workMonitor == nullptr)
			{
				workMonitor = glfwGetPrimaryMonitor();
			}
			const GLFWvidmode* vidMode = glfwGetVideoMode(workMonitor);

			//ImGui::SeparatorText("Video Mode");
			if(ImGui::TreeNode("Video Mode"))
			{

				ImGui::BulletText("screenSize: %d*%d", vidMode->width, vidMode->height);
				ImGui::BulletText("refreshRate: %d", vidMode->refreshRate);

				ImGui::TreePop();
			}



			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);
			//ImGui::Checkbox("Camera Window", &q->data->camera.bShowDebugWindow);
			ImGui::NewLine();
		}

		if (ImGui::CollapsingHeader("Graphics"))
		{
			{
				//if (ImGui::TreeNode("Basic")) {}
				const char* items[] = { "Fill", "Line", "Point" };
				ImGui::Combo("Draw Mode", &draw_mode, items, IM_ARRAYSIZE(items));
			}
			{
				//if (ImGui::TreeNode("Basic")) {}
				const char* items[] = { "Null", "Front", "Back" };
				int cull_mode = glIsEnabled(GL_CULL_FACE);
				if (cull_mode)
				{
					int mode;
					glGetIntegerv(GL_CULL_FACE_MODE, &mode);
					cull_mode = mode == GL_FRONT ? 1
								: mode == GL_BACK ? 2 : 0;
				}
					
				if (ImGui::Combo("Cull Mode", &cull_mode, items, IM_ARRAYSIZE(items)))
				{
					switch (cull_mode)
					{
					case 1:
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GL_FRONT);
					}break;
					case 2:
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GL_BACK);
					}break;
					default:
					{
						glDisable(GL_CULL_FACE);
						glCullFace(GL_BACK);
					}
					}
				}

			}
		}

		//ImGui::SliderFloat("camera speed boundary", &speed_boundary, 0.0f, 10.0f,"%.3f", ImGuiSliderFlags_AlwaysClamp);
		//ImGui::SliderFloat("camera normal speed radio", &normal_speed_ratio, 0.0f, speed_boundary, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		//ImGui::SliderFloat("camera up speed radio", &speed_up_ratio, speed_boundary, 40.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);            // Edit 1 float using a slider from 0.0f to 1.0f


		//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//	counter++;
		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);
		ImGui::NewLine();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}


}

void DisplayWindowPrivate::beginImGuiFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void DisplayWindowPrivate::endImGuiFrame()
{
	// Rendering
	ImGui::Render();
	//int display_w, display_h;
	//glfwGetFramebufferSize(window, &display_w, &display_h);
	//glViewport(0, 0, display_w, display_h);
	//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)

	const ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void DisplayWindowPrivate::destroyImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void DisplayWindowPrivate::handleErrorEvent(const int error, const char* description)
{
	std::cerr
		<< std::format("Error({}): {}", error, description)
		<< std::endl;
}

void DisplayWindowPrivate::handleFrameBufferResizeEvent(GLFWwindow* window, int width, int height)
{
	constexpr double inf = std::numeric_limits<double>::infinity();
	constexpr double nan = std::numeric_limits<double>::quiet_NaN();
	GLFWwindow* current_window = glfwGetCurrentContext();
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);
	glfwMakeContextCurrent(current_window);

	std::cout << std::format("{{{},{}}}", width, height) << std::endl;
	WindowManagerInstance->_windowMap[window]->q_func()->resizeEvent(width, height);
}

void DisplayWindowPrivate::handleCloseEvent(GLFWwindow* glfwWindow)
{
	//if (MessageBox(GetActiveWindow(), TEXT("是否关闭窗口？"), TEXT("Noticed"), MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
	//{
	//    glfwSetWindowShouldClose(gw_window, false);
	//}else
	//{
	//    glfwSetWindowShouldClose(gw_window, true);
	//}
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void DisplayWindowPrivate::handleKeyEvent(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	WindowManagerInstance->_windowMap[window]->q_func()->keyEvent(key, scanCode, action, mods);
}

void DisplayWindowPrivate::handleMouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
	WindowManagerInstance->_windowMap[window]->q_func()->mouseButtonEvent(button, action, mods);
}

// 将事件分发给对应窗口，并计算出变化量
void DisplayWindowPrivate::handleMouseMoveEvent(GLFWwindow* window, double newX, double newY)
{
	// 一定会存在
	const auto display = WindowManagerInstance->_windowMap[window]->q_func();
	const auto& displayPrivate = display->d_ptr;
	if (!displayPrivate->isFirst)
	{
		displayPrivate->lastMouseX = newX;
		displayPrivate->lastMouseY = newY;
		displayPrivate->isFirst = true;
	}
	const double lastX = displayPrivate->lastMouseX;
	const double lastY = displayPrivate->lastMouseY;

	const double deltaX = newX - lastX;
	const double deltaY = newY - lastY;

	displayPrivate->lastMouseX = newX;
	displayPrivate->lastMouseY = newY;

	display->mouseMoveEvent(
		static_cast<float>(newX)
		, static_cast<float>(newY)
		, static_cast<float>(deltaX)
		, static_cast<float>(deltaY)
	);
}

void DisplayWindowPrivate::handleScrollEvent(GLFWwindow* window, double deltaX, double deltaY)
{
	WindowManagerInstance->_windowMap[window]->q_func()->scrollEvent(static_cast<float>(deltaX), static_cast<float>(deltaY));
}
