#include "stdafx.h"
#include "actor_component.h"


void ActorComponent::BeginPlay()
{
}

void ActorComponent::EndPlay()
{
}

void ActorComponent::TickComponent(float deltaTime)
{
}

void ActorComponent::Destroy()
{
}

Actor* ActorComponent::GetOwner() const
{
	return _owner;
}

void ActorComponent::SetOwner(Actor* actor)
{
	_owner = actor;
}
