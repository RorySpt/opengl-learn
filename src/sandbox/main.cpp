#include <array>
#include <chrono>
#include <format>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <numbers>
#include <random>
#include <ranges>
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow* window = nullptr;

static void cb_frame_buffer_size(GLFWwindow* window, int width, int height)
{
}

static void cb_error(int error, const char* description)
{
}

static void cb_close(GLFWwindow* glfwWindow)
{

}

static void cb_key(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    if(window != ::window){
        return;
    }
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        {
            if(action == GLFW_RELEASE){
                glfwDestroyWindow(window);
            }
        }
        break;
    
    default:
        break;
    }
}

static void cb_mouse_move(GLFWwindow* window, double x_pos, double y_pos)
{
}

static void cb_scroll(GLFWwindow* window, double x_offset, double y_offset)
{
}



int main()
{
    glfwSetErrorCallback(cb_error);
	//初始化GLF
	if (!glfwInit())
	{
		throw(std::runtime_error(std::format("{} call glfwInit() failed! ", __FUNCTION__)));
	}
	//配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//设置主版本号为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置次版本号为3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用核心模式
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	 //创建一个窗口对象
	//comm::const_cast_ref(d->window)
	window = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	
	//注册回调函数
	glfwSetWindowCloseCallback(window, cb_close);
	glfwSetKeyCallback(window, cb_key);

	//GLAD是用来管理OpenGL的函数指针的，所以在调用任何OpenGL的函数之前初始化GLAD。
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 0;
	}
	
	//设置窗口的维度
	glViewport(0, 0, 1280, 720);
	//对窗口注册一个回调函数(Callback Function)
	glfwSetFramebufferSizeCallback(window, cb_frame_buffer_size);

	glfwSetCursorPosCallback(window, cb_mouse_move);
	glfwSetScrollCallback(window, cb_scroll);

	glfwFocusWindow(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        glfwSwapBuffers(window);
    }


	return 0;
}
