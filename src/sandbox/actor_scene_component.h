#pragma once
#include <memory>
#include <vector>

#include "actor_component.h"
#include "euler_angle_utils.h"

class SceneComponent: public ActorComponent
{
public:

	bool AttachToComponent(SceneComponent* parent);
	void DetachFromParent();

	SceneComponent* GetAttachParent() const;

	int GetNumChildrenComponents() const;
	SceneComponent* GetChildComponent(int ChildIndex) const;
	void GetChildrenComponents(std::vector<SceneComponent*>& Children);
	std::vector<SceneComponent*> GetAttachChildren();

	[[nodiscard]]
	glm::mat4 GetComponentToWorld(const glm::mat4& mat) const;
	void SetComponentToWorld(const glm::mat4& mat);
	glm::mat4 GetRelativeTransform();
	void SetRelativeTransform(const glm::mat4& mat);

	glm::vec3 _relative_location;
	EulerAngle _relative_rotation;
	glm::vec3 _relative_scale3d;

	glm::vec3 _world_location_cache;
	EulerAngle _world_rotation_cache;
	glm::vec3 _world_scale3d_cache;

	glm::mat4 _local_to_parent_cache = {};
	glm::mat4 _local_to_world = {};

	SceneComponent* _attach_parent = nullptr;
	std::vector<std::unique_ptr<SceneComponent>> _attach_children;
};

