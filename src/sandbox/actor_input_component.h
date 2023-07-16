#pragma once
#include <string_view>
#include <glm/vec2.hpp>

#include "actor_component.h"
#include "input_defines.h"
#include "input_manager.h"
#include "magic_enum.hpp"
enum EInputEvent : int
{
	IE_Pressed = 0,
	IE_Released = 1,
	IE_Repeat = 2,
	IE_DoubleClick = 3,
	IE_Axis = 4,
	IE_MAX = 5,
};

//inline KeyboardActionMapping RunForward{ "RunForward", EKeyCode::K_W, EKeyMod::Mod_Shift};


template<typename ResponseMethod>
class InputActionBinding
{
public:
	std::string actionName;
	ResponseMethod cb;
	EKeyAction action;
};


using InputActionBinding_NoParam = InputActionBinding<std::function<void()>>;
using InputActionBinding_Action = InputActionBinding<std::function<void(EKeyAction)>>;

template<typename ResponseMethod>
class InputAxisBinding
{
public:
	std::string actionName;
	ResponseMethod cb;
	glm::vec2 value;
};
using InputAxisBinding_Vec2d = InputAxisBinding<std::function<void(glm::vec2)>>;
using InputAxisBinding_Float = InputAxisBinding<std::function<void(float)>>;

class InputComponent :
    public ActorComponent
{
public:
	InputComponent();


	template<typename ResponseMethod>
		requires std::is_invocable_v<ResponseMethod>
	void BindAction(std::string_view ActionName, const EKeyAction KeyEvent, Actor* ActorObject, ResponseMethod&& Func)
	{
		if(GetInputActionKeyMappings().contains(ActionName.data()))
			NoParamActionBindings.emplace_back(ActionName.data(), std::forward<ResponseMethod>(Func), KeyEvent);
	}

	template<typename ResponseMethod>
		requires std::is_invocable_v<ResponseMethod, EKeyAction>
	void BindAction(std::string_view ActionName, Actor* ActorObject, ResponseMethod&& Func)
	{
		if (GetInputActionKeyMappings().contains(ActionName.data()))
			KeyActionActionBindings.emplace_back(ActionName.data(), std::forward<ResponseMethod>(Func));
	}

	template<typename ResponseMethod>
		requires std::is_invocable_v<ResponseMethod, glm::vec2>
	void BindAxis(std::string_view AxisName, Actor* ActorObject, ResponseMethod&& Func)
	{
		if (GetInputAxisKeyMappings().contains(AxisName.data()))
		{
			auto InputAxis = GetInputAxisKeyMappings().at(AxisName.data());
			if(InputAxis.keyCode > EKeyCode::k_PhysicalKeyEnd)
				Vec2dInputAxisBinding.emplace_back(AxisName.data(), std::forward<ResponseMethod>(Func));
		}
	}
	template<typename ResponseMethod>
		requires std::is_invocable_v<ResponseMethod, float>
	void BindAxis(std::string_view AxisName, Actor* ActorObject, ResponseMethod&& Func)
	{
		if (GetInputAxisKeyMappings().contains(AxisName.data()))
		{
			auto InputAxis = GetInputAxisKeyMappings().at(AxisName.data());
			if (InputAxis.keyCode < EKeyCode::k_PhysicalKeyEnd)
				FloatInputAxisBinding.emplace_back(AxisName.data(), std::forward<ResponseMethod>(Func));
		}
	}

	void TickComponent(float deltaTime) override;

	void RemoveBindings();



	InputManager inputManager;

	std::vector<InputActionBinding_NoParam> NoParamActionBindings;
	std::vector<InputActionBinding_Action> KeyActionActionBindings;
	std::vector<InputAxisBinding_Vec2d> Vec2dInputAxisBinding;
	std::vector<InputAxisBinding_Float> FloatInputAxisBinding;
};

