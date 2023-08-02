#pragma once
#include <type_traits>

#include "object.h"

class Actor;
class ActorComponent:public object
{
	ClassMetaDeclare(ActorComponent)
public:
	virtual ~ActorComponent() = default;
	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void TickComponent(float deltaTime);
	virtual void DestroyComponent();	// …Í«Îœ˙ªŸ

	Actor* GetOwner() const;
	void SetOwner(Actor* actor);

	void SetActive(bool b_cond);

	bool  _bIsActive = true;
	Actor* _owner;

	bool bHasBegunPlay = false;
};

template<typename ActorComponentType> requires std::is_base_of_v<ActorComponent, ActorComponentType>
bool IsValid(ActorComponentType* component)
{
	return component/*&& component->GetOwner()*/;
}
