#include "stdafx.h"
#include "IlluminantModel.h"

#include "vertices.h"


IlluminantModel::IlluminantModel()
{
	shader = ShaderProgram::makeShaderByName("lightingShader.vert", "lightingShader_fix.frag");
	shader->use();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

IlluminantModel::~IlluminantModel()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void IlluminantModel::draw(const Camera& camera, const glm::mat4& wMat)
{
	drawBegin();
	shader->glUniform("view", camera.getViewMatrix());
	shader->glUniform("projection", camera.getProjMatrix());
	shader->glUniform("model", wMat);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	drawEnd();
}

void IlluminantModel::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
{
	drawBegin();
	shader->glUniform("view", camera.getViewMatrix());
	shader->glUniform("projection", camera.getProjMatrix());
	for (auto& mat : wMats)
	{
		shader->glUniform("model", mat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	drawEnd();
}



void IlluminantModel::drawBegin()
{
	shader->use();
	shader->glUniform("lightColor", _lightColor.r, _lightColor.g, _lightColor.b);
	glBindVertexArray(VAO);
}

void IlluminantModel::drawEnd()
{
}




