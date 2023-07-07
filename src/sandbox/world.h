#pragma once
#include "light.h"
#include <map>
#include <set>

class Actor;
class World
{
public:
	virtual ~World() = default;

	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void Tick(float deltaTime);

	bool Within(Actor* actor) const;

	template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
	ActorType* SpawnActor();

	template <typename ActorType, typename GenerateActorFunc>
		requires std::is_base_of_v<Actor, ActorType>&& std::is_invocable_r_v<ActorType*, GenerateActorFunc>
	ActorType* SpawnActor(GenerateActorFunc generateActor);
	

	std::vector<Actor*> _newActors;	// 新生成的Actor列表
	std::vector<Actor*> _delActors;	// 待删除Actor列表
	std::vector<Actor*> _workActors;

	std::map<Actor*, std::unique_ptr<Actor>> _actors;





	//std::array<std::optional<LightSource>, 16> _lights;

private:
	void DealActorAdd();
	void DealActorDel();
};
inline static World GWorld;

// 声明从世界生成Actor, 由Actor实现该方法
template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
ActorType* SpawnActor(World* world = &GWorld);


template <typename ActorType> requires std::is_base_of_v<Actor, ActorType>
ActorType* World::SpawnActor()
{
	return ::SpawnActor<ActorType>(this);
}

template <typename ActorType, typename GenerateActorFunc>
	requires std::is_base_of_v<Actor, ActorType>&&std::is_invocable_r_v<ActorType*,GenerateActorFunc>
ActorType* World::SpawnActor(GenerateActorFunc generateActor)
{
	auto actor = generateActor();
	assert(actor != nullptr);
	this->_newActors.emplace_back(actor);
	return actor;
}



//// 使用局部静态变量保证初始化顺序
//inline std::shared_ptr<World>& GWorld()
//{
//	// 定义全局世界场景
//	static std::shared_ptr<World> GWorld = std::make_shared<World>();
//	return GWorld;
//}
