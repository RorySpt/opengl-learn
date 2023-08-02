#include "stdafx.h"
#include "actor_component.h"

#include "actor.h"


void ActorComponent::BeginPlay()
{
	assert(!bHasBegunPlay);
	bHasBegunPlay = true;
}

void ActorComponent::EndPlay()
{
	bHasBegunPlay = false;
}

void ActorComponent::TickComponent(float deltaTime)
{
}

void ActorComponent::DestroyComponent()
{
	if (bHasBegunPlay)
		EndPlay();
	GetOwner()->RemoveComponent(this);
}

Actor* ActorComponent::GetOwner() const
{
	return _owner;
}

void ActorComponent::SetOwner(Actor* actor)
{
	_owner = actor;
}

void ActorComponent::SetActive(bool b_cond)
{
	_bIsActive = b_cond;
}
