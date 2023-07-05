#pragma once
#include "light.h"
#include "actor.h"

#include <map>
#include <set>

class World
{
public:
	virtual ~World() = default;

	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void Tick(float deltaTime);

	template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
	std::weak_ptr<ActorType> SpawnActor();

	template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
	std::weak_ptr<ActorType> SpawnActor( std::shared_ptr<ActorType>(*generateActor)());
	

	std::vector<std::shared_ptr<Actor>> _newActors;	// 新生成的Actor列表
	std::vector<std::shared_ptr<Actor>> _delActors;	// 待删除Actor列表
	std::vector<std::shared_ptr<Actor>> _workActors;

	std::set<Actor*> _actors;
	//std::array<std::optional<LightSource>, 16> _lights;

private:
	void DealActorAdd();
	void DealActorDel();
};

template <typename ActorType> requires std::is_base_of_v<Actor, ActorType>
std::weak_ptr<ActorType> World::SpawnActor()
{
	return ::SpawnActor<ActorType>(this);
}

template <typename ActorType> requires std::is_base_of_v<Actor, ActorType>
std::weak_ptr<ActorType> World::SpawnActor(std::shared_ptr<ActorType>(* generateActor)())
{
	auto actor = generateActor();
	this->_newActors.insert(actor);
	return actor;
}

inline static World GWorld;

//// 使用局部静态变量保证初始化顺序
//inline std::shared_ptr<World>& GWorld()
//{
//	// 定义全局世界场景
//	static std::shared_ptr<World> GWorld = std::make_shared<World>();
//	return GWorld;
//}
