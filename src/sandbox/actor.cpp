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

void Actor::SetupPlayerInputComponent(InputComponent* input_component)
{
	_input_component = input_component;
}

void Actor::BeginPlay()
{
	for(const auto& component:_components)
	{
		component->BeginPlay();
	}
}

void Actor::EndPlay()
{
	for (const auto& component : _components)
	{
		component->EndPlay();
	}
}

void Actor::Tick(float deltaTime)
{
	for (const auto& component : _components)
	{
		component->TickComponent(deltaTime);
	}
}

void Actor::Destroy()
{
	_world->_delActors.emplace_back(this);
}


World* Actor::GetWorld() const
{
	return _world;
}
