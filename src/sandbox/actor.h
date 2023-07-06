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
	template<typename ActorType>
	friend std::weak_ptr<ActorType> SpawnActor(World* world);
	friend class World;
	

	Actor();
	virtual ~Actor();

	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void Tick(float deltaTime);
	virtual void Destroy();	// 申请销毁
	World* GetWorld() const;

	[[nodiscard]] const glm::mat4& localToWorld(const glm::mat4& mat) const { return _localToWorld; }
	void setLocalToWorld(const glm::mat4& mat) { _localToWorld = mat; }

protected:

	World* _world;

	std::unique_ptr<SceneComponent> _root_component;
	std::vector<std::unique_ptr<ActorComponent>> _components;

	glm::mat4 _localToWorld = {};
};


// 生成Actor的唯一方法
template<typename ActorType> requires std::is_base_of_v<Actor,ActorType>
ActorType* SpawnActor(World* world)
{
	return world->SpawnActor([world]()
	{
		// 定义生成actor的方法
		auto actor = new ActorType;
		actor->_world = world;
		return actor;
	});
}

