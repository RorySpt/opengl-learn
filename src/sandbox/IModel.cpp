#include "stdafx.h"
#include "IModel.h"

void IModel::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
{
	for(auto& mat:wMats)
	{
		draw(camera, mat);
	}
}
