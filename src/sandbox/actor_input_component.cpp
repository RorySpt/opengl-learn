#include "actor_input_component.h"

InputComponent::InputComponent()
{
	inputManager.SetEventProcessor([&](InputManager::Event e)
		{
			for(auto _binding: NoParamActionBindings)
			{
				auto& InputAction = GetInputActionKeyMappings().at(_binding.actionName);
				if(InputAction.keyCode == e.Code 
					&& !(InputAction.keyModifier & ~e.Modifiers)
					&& _binding.action == e.Action
					)
				{
					// 满足条件
					_binding.cb();
				}
			}
			for (auto _binding : KeyActionActionBindings)
			{
				auto InputAction = GetInputActionKeyMappings().at(_binding.actionName);
				if (InputAction.keyCode == e.Code 
					&& !(InputAction.keyModifier & ~e.Modifiers))
				{
					// 满足条件
					_binding.cb(e.Action);
				}
			}
		});
	inputManager.SetMouseMoveAxisProcessor([&](InputManager::Axis axis)
		{
			if (axis.deltaX == 0 && axis.deltaY == 0)return;
			for (auto _binding : Vec2dInputAxisBinding)
			{
				auto& InputAxis = GetInputAxisKeyMappings().at(_binding.actionName);
				switch (InputAxis.keyCode)
				{
				case EKeyCode::K_MouseMove_XY:
					_binding.cb({ axis.deltaX * InputAxis.Scale, axis.deltaY * InputAxis.Scale });
					break;
				case EKeyCode::K_MouseMove_X:
					_binding.cb({ axis.deltaX * InputAxis.Scale, 0 });
					break;
				case EKeyCode::K_MouseMove_Y:
					_binding.cb({ 0, axis.deltaY * InputAxis.Scale });
					break;
				default:break;
				}
			}
		});

	inputManager.SetScrollAxisProcessor([&](InputManager::Axis axis)
		{
			if (axis.deltaX == 0 && axis.deltaY == 0)return;
			for (auto _binding : Vec2dInputAxisBinding)
			{
				auto& InputAxis = GetInputAxisKeyMappings().at(_binding.actionName);
				switch (InputAxis.keyCode)
				{
				case EKeyCode::K_MouseScroll_Y:
					_binding.cb({ 0, axis.deltaY * InputAxis.Scale });
					break;
				default:break;
				}
			}
		});
}

void InputComponent::TickComponent(float deltaTime)
{
	ActorComponent::TickComponent(deltaTime);

	inputManager.tick(deltaTime);
	for (auto _binding : FloatInputAxisBinding)
	{
		const auto& InputAxis = GetInputAxisKeyMappings().at(_binding.actionName);
		if(inputManager.GetKeyState(InputAxis.keyCode) == EKeyAction::A_Press)
			_binding.cb(InputAxis.Scale * deltaTime);
	}
}

void InputComponent::RemoveBindings()
{
	NoParamActionBindings.clear();
	KeyActionActionBindings.clear();
	Vec2dInputAxisBinding.clear();
	FloatInputAxisBinding.clear();
}
