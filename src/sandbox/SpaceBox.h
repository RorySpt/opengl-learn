#pragma once
#include "IModel.h"
class SpaceBox :
    public IModel
{
	
public:
	using IModel::draw;

	void init() override;
	void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) override;



	unsigned int VAO;
	unsigned int EBO;
	unsigned int texture_id;
};

