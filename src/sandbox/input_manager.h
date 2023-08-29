#pragma once
#include <functional>
#include <map>
#include <queue>

#include "input_defines.h"

// �ܹ�����һ����Ϊ���࣬Ȼ���ܹ��󶨸���Ϊ����Ӧ����
// ���綨��һ����ΪΪMoveForward����ô���ڽ��յ�ָ���¼�����MoveForwardʱִ����Ӧ����
class InputMapping
{
public:
	std::string name; // ��������
};
class InputActionKeyMapping :public  InputMapping
{
public:
	EKeyCode keyCode;
	EKeyMods keyModifier;
};

class InputAxisKeyMapping :public InputMapping
{
public:
	EKeyCode keyCode;
	float Scale = 1.0f;
};


// ��Ҫ��̬�޸�
std::map<std::string, InputActionKeyMapping>& GetInputActionKeyMappings();
std::map<std::string, InputAxisKeyMapping>& GetInputAxisKeyMappings();


template<auto GetInputMapFunc>
class InputAdder
{
public:
	template <class... Ty>
	InputAdder(Ty&&... val)
	{
		GetInputMapFunc().emplace(std::forward<Ty>(val)...);
	}
};

#define DEF_InputAction(Name, KeyCode)\
	inline InputAdder<GetInputActionKeyMappings> InputAdder_##Name{#Name, InputActionKeyMapping{#Name, KeyCode}}
#define DEF_InputAction_WithModifier(Name, KeyCode , keyModifier)\
	inline InputAdder<GetInputActionKeyMappings> InputAdder_##Name{#Name, InputActionKeyMapping{#Name,KeyCode,keyModifier}}
#define DEF_InputAxis(Name, KeyCode , Scale)\
	inline InputAdder<GetInputAxisKeyMappings> InputAdder_##Name{#Name, InputAxisKeyMapping{ #Name, KeyCode, Scale }}

// Such as:
DEF_InputAction_WithModifier( Jump, EKeyCode::K_Space,EKeyMod::Mod_Shift);


DEF_InputAxis( LookUpDown, EKeyCode::K_MouseMove_Y, 1.0f );
DEF_InputAxis( LookLeftRight, EKeyCode::K_MouseMove_X, 1.0f);
DEF_InputAxis( LookAround, EKeyCode::K_MouseMove_XY, 1.0f);
DEF_InputAxis( MoveForward, EKeyCode::K_W, 1.0f);
DEF_InputAxis( MoveBack, EKeyCode::K_S, 1.0f);
DEF_InputAxis( MoveLeft, EKeyCode::K_A, 1.0f);
DEF_InputAxis( MoveRight, EKeyCode::K_D, 1.0f);
// �����ⲿ���룬���ں��ʵ�ʱ������
class InputManager
{
public:
	enum class EventType
	{
		Keyboard,
		MouseButton
	};
	enum class AxisType
	{
		MouseMove,
		MouseScroll
	};
	struct Event
	{
		//EventType type;
		EKeyCode Code;
		EKeyAction Action;
		EKeyMods Modifiers;
	};

	struct Axis
	{
		//AxisType type;
		float deltaX;
		float deltaY;
	};
	~InputManager();
	void EnableInput();
	void DisableInput();

	EKeyAction GetKeyState(EKeyCode code) const;

	// �ⲿ����
	void keyEvent(int keyCode, int keyAction, int keyModifiers);
	void mouseButtonEvent(int buttonCode, int keyAction, int keyModifiers);
	void mouseMoveEvent(float deltaX, float deltaY);
	void scrollEvent(float deltaX, float deltaY);

	// ��������
	void tick(float deltaTime);

	void processEvent();
	void processAxis();

	template <typename Processor> requires std::is_invocable_v<Processor, InputManager::Event>
	void SetEventProcessor(Processor processor);

	template <typename Processor> requires std::is_invocable_v<Processor, InputManager::Axis>
	void SetMouseMoveAxisProcessor(Processor processor);

	template <typename Processor> requires std::is_invocable_v<Processor, InputManager::Axis>
	void SetScrollAxisProcessor(Processor processor);

	void SetWindow(GLFWwindow* window);
	GLFWwindow* GetWindow() const;
private:
	static Axis mergeAxis(std::queue<Axis>& axes);


	bool _bEnableInput = false;
	std::queue<Event> _keyEvents;
	std::queue<Axis> _mouseMoveEvents;
	std::queue<Axis> _scrollEvents;

	std::function<void(Event)> EventProcessor;
	std::function<void(Axis)> MouseMoveAxisProcessor;
	std::function<void(Axis)> ScrollAxisProcessor;

	GLFWwindow* _window = nullptr; // ���ڲ�ѯ


	void* bind_id_mm = nullptr;
	void* bind_id_k = nullptr;
	void* bind_id_scr = nullptr;
};

template <typename Processor> requires std::is_invocable_v<Processor, InputManager::Event>
void InputManager::SetEventProcessor(Processor processor)
{
	EventProcessor = processor;
}

template <typename Processor> requires std::is_invocable_v<Processor, InputManager::Axis>
void InputManager::SetMouseMoveAxisProcessor(Processor processor)
{
	MouseMoveAxisProcessor = processor;
}

template <typename Processor> requires std::is_invocable_v<Processor, InputManager::Axis>
void InputManager::SetScrollAxisProcessor(Processor processor)
{
	ScrollAxisProcessor = processor;
}

