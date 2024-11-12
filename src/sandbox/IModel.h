#pragma once
#include <glm/glm.hpp>
#include <vector>
class Camera;
class IModel
{
public:
	virtual ~IModel()
	{
	}

	virtual void init(){}
	virtual void finished(){}
	// 绘制一次
	virtual void draw(const Camera& camera, const glm::mat4& wMat);
	// 连续绘制多次
	virtual void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) = 0;

protected:

};

