#pragma once
#include "common.h"
#include "LightBoxModel.h"
#include "IModel.h"
#include "shader.h"

class IlluminantModel :
    public IModel
{
public:
	CLASS_NO_COPY_DEFAULT_MOVE(IlluminantModel)

	IlluminantModel();
	virtual ~IlluminantModel();

	void draw(const Camera& camera, const glm::mat4& wMat) override;
	void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) override;


	void setLightColor(glm::vec3 color);
protected:
	void drawBegin();
	void drawEnd();
private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	std::shared_ptr<ShaderProgram> shader;
	glm::vec3 _lightColor;
};

inline void IlluminantModel::setLightColor(glm::vec3 color)
{
	_lightColor = color;
}
