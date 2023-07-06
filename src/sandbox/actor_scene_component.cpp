#include "stdafx.h"
#include "actor_scene_component.h"

bool SceneComponent::AttachToComponent(SceneComponent* parent)
{
	if (!IsValid(parent))return false;
	this->_attach_parent = parent;
	parent->_attach_children.emplace_back(this);
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
		throw std::exception(std::format("SceneComponent::GetChild called with a negative ChildIndex: {}", ChildIndex).c_str());
		return nullptr;
	}
	const auto& AttachedChildren = _attach_children;
	if (ChildIndex >= static_cast<int>(AttachedChildren.size()))
	{
		throw std::exception(std::format("SceneComponent::GetChild called with an out of range ChildIndex: {}; Number of children is {}.", ChildIndex, AttachedChildren.size()).c_str());
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

glm::mat4 SceneComponent::GetComponentToWorld(const glm::mat4& mat) const
{
	return _local_to_world;
}

void SceneComponent::SetComponentToWorld(const glm::mat4& mat)
{
	_local_to_world = mat;
}

glm::mat4 SceneComponent::GetRelativeTransform()
{
	return _local_to_parent_cache;
}

void SceneComponent::SetRelativeTransform(const glm::mat4& mat)
{
	_local_to_parent_cache = mat;
}
