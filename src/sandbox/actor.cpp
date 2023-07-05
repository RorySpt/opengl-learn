#include "stdafx.h"
#include "actor.h"
#include "World.h"

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
	_world->_delActors.emplace(this);
}

World* Actor::GetWorld()
{
	return _world;
}
