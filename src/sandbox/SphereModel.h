#pragma once
#include "common.h"

#include "material.h"
#include "IModel.h"
#include "light.h"



class SphereModel :IModel
{
public:
	using IModel::draw;
	struct VertexInfo
	{
		glm::vec3 location;
		glm::vec3 normal;
	};
	SphereModel();

	void UpdateBufferData() const
	{
		glNamedBufferData(VBO, vertexes.size() * sizeof VertexInfo, vertexes.data(), GL_DYNAMIC_DRAW);
		glNamedBufferData(EBO, indexes.size() * sizeof(GLuint), indexes.data(), GL_DYNAMIC_DRAW);
	}
	void initSphereVertexes(float radius, int segment = 10);

	void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) override;

	Material3 _material;
	std::vector<LightSource> _lights;
private:
	GLuint EBO;
	GLuint VBO;
	GLuint VAO;

	std::vector<VertexInfo> vertexes;
	std::vector<GLuint> indexes;
	std::shared_ptr<ShaderProgram> shader;
};

