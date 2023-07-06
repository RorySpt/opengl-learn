#include "stdafx.h"
#include "actor.h"
#include "actor_component.h"
#include "actor_scene_component.h"
#include "World.h"

Actor::Actor()
{
	_root_component = std::make_unique<SceneComponent>();
	_root_component->SetOwner(this);
}

Actor::~Actor()
{
	//_world->_actors.erase(this);
}

void Actor::BeginPlay()
{
}

void Actor::EndPlay()
{
}

void Actor::Tick(float deltaTime)
{
	
}

void Actor::Destroy()
{
	_world->_delActors.emplace_back(this);
}


World* Actor::GetWorld() const
{
	return _world;
}
