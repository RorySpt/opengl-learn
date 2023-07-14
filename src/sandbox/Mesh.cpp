#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = std::move(vertices);
	this->indices = std::move(indices);
	this->textures = std::move(textures);

	setupMesh();
}
void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// 顶点位置
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	// 顶点法线
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));

	// 顶点纹理坐标
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

	glBindVertexArray(0);
}
void Mesh::Draw(const ShaderProgram& shader) const
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	shader.glUniform("material.""enable_""diffuseTex", true);
	shader.glUniform("material.""enable_""specularTex", true);
	shader.glUniform("material.""enable_""emissionTex", true);
	shader.glUniform("material.""diffuseTex", 0);
	shader.glUniform("material.""specularTex", 0);
	shader.glUniform("material.""emissionTex", 0);
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
		std::string number;
		std::string name = textures[i].type;
		if (name == "diffuseTex")
			number = std::to_string(diffuseNr++);
		else if (name == "specularTex")
			number = std::to_string(specularNr++);
		else
		{
			std::cout << __FUNCTION__ << " UNKNOWN_TYPE " + name << std::endl;
			continue;
		}
		if(number == "1")
		{
			shader.glUniform("material.""enable_" + name, true);
			shader.glUniform("material." + name, static_cast<int>(i));
		}
		shader.glUniform("material." + name.append(number), static_cast<int>(i));
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// 绘制网格
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}


