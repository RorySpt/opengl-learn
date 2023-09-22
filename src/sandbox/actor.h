#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "world.h"
#include "actor_component.h"
#include "object.h"


class World;
class ActorComponent;
class SceneComponent;
class InputComponent;

//template<typename T>
//concept ComponentType = std::is_base_of_v<ActorComponent, T>;

class Actor: public object
{

public:
	template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
	friend ActorType* SpawnActor(World* world);
	friend class World;

	bool bAutoActiveCamera = true;

	Actor();
	virtual ~Actor();

	virtual void OnConstruct();

	virtual void SetupPlayerInputComponent(InputComponent *input_component);

	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void Tick(float deltaTime);
	virtual void Destroy();	// 申请销毁
	World* GetWorld() const;

	virtual void UI_Draw() {};

	template<typename ComponentType> requires std::is_base_of_v<ActorComponent, ComponentType>
	ComponentType* CreateDefaultComponent();

	void RemoveComponent(ActorComponent*);
	void ProcessComponentRemove();
	void ProcessComponentAdd();

	template<typename ComponentType> requires std::is_base_of_v<ActorComponent, ComponentType>
	std::vector<ComponentType*> GetComponentsByType();
//protected:

	World* _world = nullptr;
	InputComponent* _input_component = nullptr;
	SceneComponent* _root_component;
	std::vector<ActorComponent*> _non_scene_components;
	std::vector<std::unique_ptr<ActorComponent>> _owned_components;

	std::vector<ActorComponent*> _components_need_add;
	std::vector<ActorComponent*> _components_need_del;

	std::map<std::size_t, std::vector<ActorComponent*>> _owned_components_map;

	DisplayNameGenerator _display_name_generator;
};

template <typename ComponentType> requires std::is_base_of_v<ActorComponent, ComponentType>
ComponentType* Actor::CreateDefaultComponent()
{
	_components_need_add.emplace_back(new ComponentType);
	auto component = static_cast<ComponentType*>(_components_need_add.back());
	component->SetOwner(this);
	component->set_self_display_name_generator(&_display_name_generator);
	component->set_name(component->type_name());
	if(_world!=nullptr)component->OnConstruct();
	return component;
}

template <typename ComponentType> requires std::is_base_of_v<ActorComponent, ComponentType>
std::vector<ComponentType*> Actor::GetComponentsByType()
{
	//if(const auto hashCode = typeid(ComponentType).hash_code();
	//	_owned_components_map.contains(hashCode))
	//{
	//	return _owned_components_map[hashCode];
	//}
	//std::vector<ComponentType*> components;
	//for(auto &com: _owned_components)
	//{
	//	if(auto actual = dynamic_cast<ComponentType*>(com.get()))
	//	{
	//		components.push_back(actual);
	//	}
	//}
	//std::vector<ComponentType*> (
	//	);
	return _owned_components | std::views::transform([](const std::unique_ptr<ActorComponent>& com)
		{
			return dynamic_cast<ComponentType*>(com.get());
		}) | std::views::filter([](const ComponentType* com)->bool
		{
			return com;
		}) | std::ranges::to<std::vector<ComponentType*>>();
}


// 生成Actor的唯一方法
template<typename ActorType> requires std::is_base_of_v<Actor,ActorType>
ActorType* SpawnActor(World* world)
{
	return world->SpawnActor<ActorType>([world, actor_world_address  = &ActorType::_world]() ->ActorType*
	{
		// 定义生成actor的方法
		auto actor = new ActorType;
		actor->*actor_world_address = world;
		return actor;
	});
}

