#include "stdafx.h"
#include "IModel.h"

void IModel::draw(const Camera& camera, const glm::mat4& wMat)
{
	draw(camera, std::vector{ wMat });
}


