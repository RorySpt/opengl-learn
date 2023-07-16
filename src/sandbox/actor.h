#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "world.h"
#include "actor_component.h"

class World;
class ActorComponent;
class SceneComponent;
class InputComponent;
class Actor
{
public:
	template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
	friend ActorType* SpawnActor(World* world);
	friend class World;
	

	Actor();
	virtual ~Actor();

	virtual void SetupPlayerInputComponent(InputComponent *input_component);

	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void Tick(float deltaTime);
	virtual void Destroy();	// ��������
	World* GetWorld() const;


protected:

	World* _world;
	InputComponent* _input_component = nullptr;
	std::unique_ptr<SceneComponent> _root_component;
	std::vector<std::unique_ptr<ActorComponent>> _components;
};


// ����Actor��Ψһ����
template<typename ActorType> requires std::is_base_of_v<Actor,ActorType>
ActorType* SpawnActor(World* world)
{
	return world->SpawnActor<ActorType>([world, actor_world_address  = &ActorType::_world]()
	{
		// ��������actor�ķ���
		auto actor = new ActorType;
		actor->*actor_world_address = world;
		return actor;
	});
}

