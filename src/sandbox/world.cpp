#include "stdafx.h"
#include "world.h"
#include "actor.h"
#include "player_controller.h"

World::World()
{
	_playerController = SpawnActor<PlayerController>();
}

void World::init(GLFWwindow* w)
{
	_playerController->_input_component->inputManager.SetWindow(w);
}

void World::BeginPlay()
{
	assert(!bHasBegunPlay);
	bHasBegunPlay = true;

	DealActorDel();
	DealActorAdd();

	for (const auto& actor : _workActors)
	{
		actor->BeginPlay();
	}
}

void World::EndPlay()
{
	for (const auto& actor : _workActors)
	{
		actor->EndPlay();
	}
}

void World::Tick(float deltaTime)
{

	DealActorDel();
	DealActorAdd();

	for (const auto& actor : _workActors)
	{
		actor->Tick(deltaTime);
	}
}

bool World::Within(Actor* actor) const
{
	return _workActors.empty() ? _actors.contains(actor)
	: _actors.contains(actor) || std::ranges::find(_workActors , actor) != std::end(_workActors);
}

PlayerController* World::GetPlayerController() const
{
	return _playerController;
}

std::vector<LightSource> World::GetLightsByType(LightSource::LightType t_light) const
{
	std::vector<LightSource> lights;
	for(auto op_light:_lights)
	{
		if (op_light.has_value() && op_light->type == t_light)
			lights.emplace_back(*op_light);
	}
	return lights;
}


void World::DealActorAdd()
{
	if (!_newActors.empty())
	{
		_workActors.insert_range(std::end(_workActors), std::move(_newActors));
		_actors.insert_range(_workActors 
			| std::views::transform([](Actor* actor){
				return std::pair{ actor, std::unique_ptr<Actor>()};
			}));

		_newActors.clear();
	}
}

void World::DealActorDel()
{
	
	if (!_delActors.empty())
	{
		std::erase_if(_workActors, [&](Actor* actor)
			{
				return std::ranges::find(_delActors, actor) != std::end(_delActors);
			});
		for (const auto actor : _delActors)
		{
			// ´¥·¢actorÏú»Ù
			_actors.erase(actor);
		}

		_delActors.clear();
	}
}

World GWorld;