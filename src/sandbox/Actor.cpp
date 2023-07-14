#include "stdafx.h"
#include "Actor.h"
#include "World.h"
Actor::Actor(World* world): _world(world)
{
	_world->_actors.insert(this);
}

Actor::~Actor()
{
	_world->_actors.erase(this);
}
