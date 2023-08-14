#pragma once
#include "stdafx.h"
#include <map>
#include <set>
#include <memory>

#include "light.h"
#include "object.h"


class Actor;
class PlayerController;
class World final
{
public:
	World();
	virtual ~World() = default;

	void init(GLFWwindow* w) ;

	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void Tick(float deltaTime);

	bool Within(Actor* actor) const;

	template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
	ActorType* SpawnActor();

	template <typename ActorType, typename GenerateActorFunc>
		requires std::is_base_of_v<Actor, ActorType>&& std::is_invocable_r_v<ActorType*, GenerateActorFunc>
	ActorType* SpawnActor(GenerateActorFunc generateActor);
	
	PlayerController* GetPlayerController() const;
	
	std::vector<LightSource> GetLightsByType(LightSource::LightType t_light) const;


	std::vector<Actor*> _newActors;	// �����ɵ�Actor�б�
	std::vector<Actor*> _delActors;	// ��ɾ��Actor�б�
	std::vector<Actor*> _workActors;

	std::map<Actor*, std::unique_ptr<Actor>> _actors;


	PlayerController* _playerController;

	bool bHasBegunPlay = false;

	std::array<std::optional<LightSource>, 16> _lights;

	DisplayNameGenerator display_name_generator;
private:
	void DealActorAdd();
	void DealActorDel();
};
extern World GWorld;

// ��������������Actor, ��Actorʵ�ָ÷���
template<typename ActorType> requires std::is_base_of_v<Actor, ActorType>
ActorType* SpawnActor(World* world);


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
	actor->set_self_display_name_generator(&display_name_generator);
	actor->set_name(actor->type_name());
	this->_newActors.emplace_back(actor);
	actor->OnConstruct();
	return actor;
}
template<typename ActorComponentType> requires std::is_base_of_v<Actor, ActorComponentType>
bool IsValid(Actor* actor)
{
	return actor;
}


//// ʹ�þֲ���̬������֤��ʼ��˳��
//inline std::shared_ptr<World>& GWorld()
//{
//	// ����ȫ�����糡��
//	static std::shared_ptr<World> GWorld = std::make_shared<World>();
//	return GWorld;
//}
