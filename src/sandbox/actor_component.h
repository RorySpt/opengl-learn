#pragma once
#include <type_traits>
class Actor;
class ActorComponent
{
public:
	virtual ~ActorComponent() = default;
	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void TickComponent(float deltaTime);
	virtual void Destroy();	// …Í«Îœ˙ªŸ

	Actor* GetOwner() const;
	void SetOwner(Actor* actor);


	Actor* _owner;
};

template<typename ActorComponentType> requires std::is_base_of_v<ActorComponent, ActorComponentType>
bool IsValid(ActorComponentType* component)
{
	return component/*&& component->GetOwner()*/;
}
