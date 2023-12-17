#include "stdafx.h"
#include "actor.h"
#include "actor_component.h"
#include "actor_scene_component.h"
#include "World.h"

Actor::Actor()
{
	_root_component = CreateDefaultComponent<SceneComponent>();
}

Actor::~Actor()
{
	//_world->_actors.erase(this);
}

void Actor::OnConstruct()
{
	for (const auto& component : _components_need_add)
	{
		component->OnConstruct();
	}
}

void Actor::SetupPlayerInputComponent(InputComponent* input_component)
{
	_input_component = input_component;
}

void Actor::BeginPlay()
{
	// 移除非Root的根Scene组件;
	std::erase_if(_owned_components, [&](const std::unique_ptr<ActorComponent>& component)
		{
			const auto scene = dynamic_cast<SceneComponent*>(component.get());
			return scene && scene->GetAttachParent() == nullptr && _root_component != scene;
		});
	for(const auto& component:_owned_components)
	{
		component->BeginPlay();
	}
}

void Actor::EndPlay()
{
	for (const auto& component : _owned_components)
	{
		component->EndPlay();
	}
}

void Actor::Tick(float deltaTime)
{
	ProcessComponentRemove();
	ProcessComponentAdd();
	for (const auto& component : _owned_components)
	{
		if (!component->bHasBegunPlay) BeginPlay();
		component->TickComponent(deltaTime);
	}
}

void Actor::Destroy()
{
	_world->_delActors.emplace_back(this);
}


World* Actor::GetWorld() const
{
	return _world;
}

void Actor::RemoveComponent(ActorComponent* component)
{
	_components_need_del.emplace_back(component);
}

void Actor::ProcessComponentRemove()
{
	using value_type = decltype(_owned_components)::value_type;

	if(_components_need_del.empty()) return;

	for(auto component: _components_need_del)
	{
		if (component == _root_component)
		{
			_root_component = nullptr;
		}
		else if (const auto scene = dynamic_cast<SceneComponent*>(component))
		{
			scene->DetachFromParent();
		}else
		{
			std::erase(_non_scene_components, component);
		}
		const auto result = std::erase_if(_owned_components, [component](const value_type& _component)
			{
				return _component.get() == component;
			});

		//if(auto hashCode = typeid(*component).hash_code();_owned_components_map.contains(hashCode))
		//{
		//	std::erase_if(_owned_components_map[hashCode], [component](const value_type::element_type* _component)
		//		{
		//			return _component == component;
		//		});
		//}
	}
	_components_need_del.clear();
}

void Actor::ProcessComponentAdd()
{
	using value_type = decltype(_owned_components)::value_type;

	if (_components_need_add.empty()) return;
	for (auto component : _components_need_add)
	{

		if (const auto scene = dynamic_cast<SceneComponent*>(component))
		{
			
		}else
		{
			_non_scene_components.emplace_back(component);
		}
		_owned_components.emplace_back(component);
		//_owned_components_map[typeid(*component).hash_code()].emplace_back(component);
	}
	_components_need_add.clear();


}
