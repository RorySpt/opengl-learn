#pragma once
#include <memory>
#include <vector>

#include "actor_component.h"
#include "euler_angle_utils.h"

class SceneComponent: public ActorComponent
{
public:
	virtual void TickComponent(float deltaTime) override;

	bool AttachToComponent(SceneComponent* parent);
	void DetachFromParent();

	SceneComponent* GetAttachParent() const;

	int GetNumChildrenComponents() const;
	SceneComponent* GetChildComponent(int ChildIndex) const;
	void GetChildrenComponents(std::vector<SceneComponent*>& Children);
	std::vector<SceneComponent*> GetAttachChildren();

	[[nodiscard]]
	glm::mat4 GetComponentToWorld() const;
	void SetComponentToWorld(const glm::mat4& mat);
	glm::mat4 GetRelativeTransform() const;
	void SetRelativeTransform(const glm::mat4& mat);

	void SetRelativeLocation(glm::vec3 relativeLocation);
	glm::vec3 GetRelativeLocation()const;

	void SetRelativeRotation(glm::quat relativeRotation);
	glm::quat GetRelativeRotation()const;

	void SetRelativeScale3d(glm::vec3 relativeScale3d);
	glm::vec3 GetRelativeScale3d()const;

	

	void ParentTransformChanged();
	void LocalToWorldChanged();
	void RelativeTransformChanged();
	void RelativeLocationChanged();
	void RelativeRotationChanged();
	void RelativeScale3dChanged();

	glm::vec3 _relative_location;
	glm::quat _relative_rotation;
	glm::vec3 _relative_scale3d;

	glm::vec3 _world_location_cache;
	glm::quat _world_rotation_cache;
	glm::vec3 _world_scale3d_cache;

	glm::mat4 _local_to_parent_cache;
	glm::mat4 _local_to_world;

	//bool _local_to_world_changed = false;


	SceneComponent* _attach_parent = nullptr;
	std::vector<std::unique_ptr<SceneComponent>> _attach_children;
};

