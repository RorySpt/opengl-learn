#pragma once
#include <glm/glm.hpp>

#include "world.h"

class World;
class Actor
{
public:
	template<typename ActorType>
	friend std::weak_ptr<ActorType> SpawnActor(World* world);
	friend class World;
	

	Actor() = default;
	virtual ~Actor();

	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void Tick(float deltaTime);
	virtual void Destroy();

	World* GetWorld();

	[[nodiscard]] const glm::mat4& localToWorld(const glm::mat4& mat) const { return _localToWorld; }
	void setLocalToWorld(const glm::mat4& mat) { _localToWorld = mat; }

protected:

	World* _world;
	glm::mat4 _localToWorld = {};
};


// 生成Actor的唯一方法
template<typename ActorType> requires std::is_base_of_v<Actor,ActorType>
std::weak_ptr<ActorType> SpawnActor(World* world = &GWorld)
{
	return world->SpawnActor([world]()
	{
		// 定义生成actor的方法
		auto actor = std::make_shared<ActorType>();
		actor->_world = world;
		return actor;
	});
}

