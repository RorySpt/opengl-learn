#pragma once
#include "actor_scene_component.h"
#include "IModel.h"

class PrimitiveComponent :
    public SceneComponent
{
    ClassMetaDeclare(PrimitiveComponent)
public:
	virtual void draw(const Camera& camera) = 0;
};

