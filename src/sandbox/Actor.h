#pragma once
#include <glm/glm.hpp>

class World;
class Actor
{
public:
	explicit Actor(World* world);
	virtual ~Actor();

	[[nodiscard]] const glm::mat4& localToWorld(const glm::mat4& mat) const { return _localToWorld; }
	void setLocalToWorld(const glm::mat4& mat) { _localToWorld = mat; }
protected:

	World* _world;
	glm::mat4 _localToWorld = {};
};

