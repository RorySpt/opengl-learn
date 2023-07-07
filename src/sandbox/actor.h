#pragma once
#include <glm/glm.hpp>

#include "world.h"
#include "actor_component.h"

class World;
class ActorComponent;
class SceneComponent;
class Actor
{
public:
	template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
	friend ActorType* SpawnActor(World* world);
	friend class World;
	

	Actor();
	virtual ~Actor();

	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void Tick(float deltaTime);
	virtual void Destroy();	// 申请销毁
	World* GetWorld() const;


protected:

	World* _world;

	std::unique_ptr<SceneComponent> _root_component;
	std::vector<std::unique_ptr<ActorComponent>> _components;
};


// 生成Actor的唯一方法
template<typename ActorType> requires std::is_base_of_v<Actor,ActorType>
ActorType* SpawnActor(World* world)
{
	return world->SpawnActor<ActorType>([world, actor_world_address  = &ActorType::_world]()
	{
		// 定义生成actor的方法
		auto actor = new ActorType;
		actor->*actor_world_address = world;
		return actor;
	});
}

