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
	_playerController->GetInputManager()->SetWindow(w);
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

std::vector<LightSource> World::GetLightsByTypeWithAllChannel(LightType t_light) const
{
	std::vector<LightSource> lights;
	for(const auto& channel_lights:_lights)
	{
		for(auto &light: channel_lights)
		{
			if(light->type == t_light)
			lights.emplace_back(*light);
		}
	}
	return lights;
}

std::vector<LightSource> World::GetLightsByType(LightType t_light, int channel) const
{
	assert(channel < static_cast<int>(_lights.size()));
	if (channel < 0)channel = 0;

	std::vector<LightSource> lights;
	for (auto& light : _lights[channel])
	{
		if (light->type == t_light)
			lights.emplace_back(*light);
	}
	return lights;
}

std::vector<LightSource> World::GetLightsByChannel(int channel) const
{
	assert(channel < static_cast<int>(_lights.size()));
	if (channel < 0)channel = 0;

	std::vector<LightSource> lights;
	lights.reserve(_lights[channel].size());
	for (auto& light : _lights[channel])
	{
		lights.emplace_back(*light);
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