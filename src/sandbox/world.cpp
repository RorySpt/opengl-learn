#include "stdafx.h"
#include "world.h"

void World::BeginPlay()
{
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

void World::DealActorAdd()
{
	if (!_newActors.empty())
	{
		_workActors.insert_range(std::end(_workActors), std::move(_newActors));
		_actors.insert_range(_workActors 
			| std::views::transform([](const std::shared_ptr<Actor>& actor){
				return actor.get();
			}));

		_newActors.clear();
	}
}

void World::DealActorDel()
{
	
	if (!_delActors.empty())
	{
		std::ranges::remove_if(_workActors, [&](std::shared_ptr<Actor>& actor)
			{
				return std::ranges::find(_delActors, actor) != std::end(_delActors);
			});
		for (const auto& actor : _delActors)
		{
			_actors.erase(actor.get());
		}

		_delActors.clear();
	}
}
