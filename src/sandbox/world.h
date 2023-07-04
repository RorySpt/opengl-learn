#pragma once
#include "light.h"
#include "actor.h"

#include <map>
#include <set>
class World
{
public:




	std::set<Actor*> _actors;
	std::array<std::optional<LightSource>, 16> _lights;
};
