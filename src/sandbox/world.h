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

	template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
	ActorType* SpawnActor(ActorType* (*generateActor)());
	

	std::vector<Actor*> _newActors;	// �����ɵ�Actor�б�
	std::vector<Actor*> _delActors;	// ��ɾ��Actor�б�
	std::vector<Actor*> _workActors;

	std::map<Actor*, std::unique_ptr<Actor>> _actors;





	//std::array<std::optional<LightSource>, 16> _lights;

private:
	void DealActorAdd();
	void DealActorDel();
};
inline static World GWorld;

// ��������������Actor, ��Actorʵ�ָ÷���
template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
ActorType* SpawnActor(World* world = &GWorld);


template <typename ActorType> requires std::is_base_of_v<Actor, ActorType>
ActorType* World::SpawnActor()
{
	return ::SpawnActor<ActorType>(this);
}

template <typename ActorType> requires std::is_base_of_v<Actor, ActorType>
ActorType* World::SpawnActor(ActorType* (* generateActor)())
{
	auto actor = generateActor();
	assert(actor != nullptr);
	this->_newActors.insert(actor);
	return actor;
}



//// ʹ�þֲ���̬������֤��ʼ��˳��
//inline std::shared_ptr<World>& GWorld()
//{
//	// ����ȫ�����糡��
//	static std::shared_ptr<World> GWorld = std::make_shared<World>();
//	return GWorld;
//}
