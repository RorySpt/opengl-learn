#include "stdafx.h"
#include "actor_scene_component.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/matrix_decompose.hpp>
void SceneComponent::TickComponent(float deltaTime)
{
	ActorComponent::TickComponent(deltaTime);

}

bool SceneComponent::AttachToComponent(SceneComponent* parent)
{
	if (!IsValid(parent))return false;
	this->_attach_parent = parent;
	parent->_attach_children.emplace_back(this);

	ParentTransformChanged();

	return true;
}

void SceneComponent::DetachFromParent()
{
	if (!IsValid(_attach_parent))return;

	const auto result = 
		std::ranges::find_if(_attach_parent->_attach_children, [&](auto& component)
		{
			return component.get() == this;
		});
	if (result != _attach_parent->_attach_children.end())
		_CRT_UNUSED(result->release());
	_attach_parent->_attach_children.erase(result);
	_attach_parent = nullptr;

	ParentTransformChanged();
}

SceneComponent* SceneComponent::GetAttachParent() const
{
	return _attach_parent;
}

int SceneComponent::GetNumChildrenComponents() const
{
	return static_cast<int>(_attach_children.size());
}

SceneComponent* SceneComponent::GetChildComponent(int ChildIndex) const
{
	if (ChildIndex < 0)
	{
		throw std::out_of_range(std::format("SceneComponent::GetChild called with a negative ChildIndex: {}", ChildIndex).c_str());
		return nullptr;
	}
	const auto& AttachedChildren = _attach_children;
	if (ChildIndex >= static_cast<int>(AttachedChildren.size()))
	{
		throw std::out_of_range(std::format("SceneComponent::GetChild called with an out of range ChildIndex: {}; Number of children is {}.", ChildIndex, AttachedChildren.size()).c_str());
		return nullptr;
	}
	return AttachedChildren[ChildIndex].get();
}

void SceneComponent::GetChildrenComponents(std::vector<SceneComponent*>& Children)
{
	Children.clear();
	Children.insert_range(Children.end(), _attach_children 
		| std::ranges::views::transform([](auto& component)
		{
			return component.get();
		}));
}

std::vector<SceneComponent*> SceneComponent::GetAttachChildren()
{
	auto children_view = _attach_children
		| std::ranges::views::transform([](auto& component)
			{
				return component.get();
			});
	
	return { std::from_range, children_view };
}

glm::mat4 SceneComponent::GetComponentToWorld() const
{
	return _local_to_world;
}

void SceneComponent::SetComponentToWorld(const glm::mat4& mat)
{
	_local_to_world = mat;
	LocalToWorldChanged();
}

glm::mat4 SceneComponent::GetRelativeTransform() const
{
	return _local_to_parent_cache;
}

void SceneComponent::SetRelativeTransform(const glm::mat4& mat)
{
	_local_to_parent_cache = mat;
	
	RelativeTransformChanged();
}

void SceneComponent::SetRelativeLocation(glm::vec3 relativeLocation)
{
	_relative_location = relativeLocation;
	RelativeLocationChanged();
}

glm::vec3 SceneComponent::GetRelativeLocation() const
{
	return _relative_location;
}

void SceneComponent::SetRelativeRotation(glm::quat relativeRotation)
{
	_relative_rotation = relativeRotation;
	RelativeRotationChanged();
}

glm::quat SceneComponent::GetRelativeRotation() const
{
	return _relative_rotation;
}

void SceneComponent::SetRelativeScale3d(glm::vec3 relativeScale3d)
{
	_relative_scale3d = relativeScale3d;
}

glm::vec3 SceneComponent::GetRelativeScale3d() const
{
	return _relative_scale3d;
}


void SceneComponent::ParentTransformChanged()
{
	if(IsValid(_attach_parent))
	{
		_local_to_world = _attach_parent->GetComponentToWorld() * GetRelativeTransform();

		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(_local_to_world, _world_scale3d_cache, _world_rotation_cache, _world_location_cache, skew, perspective);

	}else
	{
		_local_to_world = _local_to_parent_cache;
		_world_location_cache = _relative_location;
		_world_rotation_cache = _relative_rotation;
		_world_scale3d_cache = _relative_scale3d;
	}
	for(const auto& child: _attach_children)
	{
		child->ParentTransformChanged();
	}
}

void SceneComponent::LocalToWorldChanged()
{
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(_local_to_world, _world_scale3d_cache, _world_rotation_cache, _world_location_cache, skew, perspective);
	if (IsValid(_attach_parent))
	{
		_local_to_parent_cache = glm::inverse(_attach_parent->GetComponentToWorld()) * _local_to_world;
	}else
	{
		_local_to_parent_cache = _local_to_world;
	}

	glm::decompose(_local_to_parent_cache, _relative_scale3d, _relative_rotation, _relative_location, skew, perspective);

	for (const auto& child : _attach_children)
	{
		child->ParentTransformChanged();
	}
}

void SceneComponent::RelativeTransformChanged()
{
	if (IsValid(_attach_parent))
	{
		_local_to_world = _attach_parent->GetComponentToWorld() * _local_to_parent_cache;

		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(_local_to_world, _world_scale3d_cache, _world_rotation_cache, _world_location_cache, skew, perspective);
	}else
	{
		_local_to_world = _local_to_parent_cache;
		_world_location_cache = _relative_location;
		_world_rotation_cache = _relative_rotation;
		_world_scale3d_cache = _relative_scale3d;
	}

	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(_local_to_parent_cache, _relative_scale3d, _relative_rotation, _relative_location, skew, perspective);

	for (const auto& child : _attach_children)
	{
		child->ParentTransformChanged();
	}
}

void SceneComponent::RelativeLocationChanged()
{
	_local_to_parent_cache =
		glm::translate(glm::mat4(1.0f), _relative_scale3d)
		* glm::mat4_cast(_relative_rotation)
		* glm::scale(glm::mat4(1.0f), _relative_scale3d);

	if (IsValid(_attach_parent))
	{
		_local_to_world = _attach_parent->GetComponentToWorld() * _local_to_parent_cache;

		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(_local_to_world, _world_scale3d_cache, _world_rotation_cache, _world_location_cache, skew, perspective);
	}
	else
	{
		_local_to_world = _local_to_parent_cache;
		_world_location_cache = _relative_location;
		_world_rotation_cache = _relative_rotation;
		_world_scale3d_cache = _relative_scale3d;
	}

	for (const auto& child : _attach_children)
	{
		child->ParentTransformChanged();
	}
}

void SceneComponent::RelativeRotationChanged()
{
	RelativeLocationChanged();
}

void SceneComponent::RelativeScale3dChanged()
{
	RelativeLocationChanged();
}