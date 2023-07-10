#include "stdafx.h"
#include "LightBoxModel.h"
#include "common.h"
#include "vertices.h"
LightBoxModel::LightBoxModel()
{
	
	init();
}

LightBoxModel::~LightBoxModel()
{
	finished();
}

void LightBoxModel::init()
{
	shader = ShaderProgram::makeShaderByName("lightingShader.vert","lightingShader.frag");
	shader->use();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));  // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	_material = MaterialTable::instance()->getMaterial("default");
}

void LightBoxModel::finished()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void LightBoxModel::draw(const Camera& camera, const glm::mat4& wMat)
{
	drawBegin();
	shader->glUniform("viewPos", camera.Position);
	shader->glUniform("view", camera.getViewMatrix());
	shader->glUniform("projection", camera.getProjMatrix());
	shader->glUniform("model", wMat);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	drawEnd();
}

void LightBoxModel::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
{
	drawBegin();
	shader->glUniform("viewPos", camera.Position);
	shader->glUniform("view", camera.getViewMatrix());
	shader->glUniform("projection", camera.getProjMatrix());
	for(auto& mat: wMats)
	{
		shader->glUniform("model", mat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	drawEnd();
}

void LightBoxModel::drawBegin()
{
	shader->use();

	shader->glUniform("light.position", _light.position);
	shader->glUniform("light.ambient", _light.ambient);
	shader->glUniform("light.diffuse", _light.diffuse);
	shader->glUniform("light.specular", _light.specular);

	shader->glUniform("material.ambient", _material.ambient);
	shader->glUniform("material.diffuse", _material.diffuse);
	shader->glUniform("material.specular", _material.specular);
	shader->glUniform("material.shininess", _material.shininess);

	glBindVertexArray(VAO);
}

void LightBoxModel::drawEnd()
{
}
unsigned GetRandomRabbitTexture()
{
	//namespace fs = std::filesystem;
	//constexpr std::string_view RabbitDir = "C:/Users/zhang/Pictures/Material/Rabbit";
	//
	//static std::vector<fs::path> rabbitPics;
	//static std::random_device rd;
	//static std::default_random_engine dre(rd());
	//if(rabbitPics.empty())
	//for (auto& p : fs::directory_iterator(RabbitDir))
	//{
	//	if(p.path().has_extension())
	//	{
	//		rabbitPics.emplace_back(p.path());
	//	}
	//}
	//if(!rabbitPics.empty())
	//{
	//	std::uniform_int_distribution<> uid(0, rabbitPics.size() - 1);
	//	int index = uid(dre);
	//	return comm::loadTexture(fs::absolute(rabbitPics[index]).string());
	//}
	return comm::loadTexture(std::string(comm::dir_picture) + "/matrix.jpg");
}
LightBoxModel2::LightBoxModel2()
{
	shader = std::make_shared<ShaderProgram>(std::string(comm::dir_shader) + "/lightingShader.vert", std::string(comm::dir_shader) + "/lightingShader3.frag");
	shader->use();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices2, vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));  // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));  // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	auto container2_diffuse = comm::loadTexture(std::string(comm::dir_picture) + "/container2.png");
	auto container2_specular = comm::loadTexture(std::string(comm::dir_picture) + "/container2_specular.png");

	setMaterial({ GetRandomRabbitTexture(),
		container2_diffuse
		,container2_specular
		,32.0f });
}

LightBoxModel2::~LightBoxModel2()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void LightBoxModel2::draw(const Camera& camera, const glm::mat4& wMat)
{
	drawBegin();
	shader->glUniform("viewPos", camera.Position);
	shader->glUniform("view", camera.getViewMatrix());
	shader->glUniform("projection", camera.getProjMatrix());
	shader->glUniform("model", wMat);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	drawEnd();
}

void LightBoxModel2::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
{
	drawBegin();

	shader->glUniform("viewPos", camera.Position);
	shader->glUniform("view", camera.getViewMatrix());
	shader->glUniform("projection", camera.getProjMatrix());
	for(auto& mat:wMats)
	{
		shader->glUniform("model", mat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	

	drawEnd();
}

void LightBoxModel2::drawBegin()
{
	shader->use();
	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, std::get<Material3::TextureID>(_material.diffuse));
	shader->glUniform("material.diffuseTex", 0);
	shader->glUniform("material.enable_diffuseTex", true);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, std::get<Material3::TextureID>(_material.specular));
	shader->glUniform("material.specularTex", 1);
	shader->glUniform("material.enable_specularTex", true);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, std::get<Material3::TextureID>(_material.emission));
	shader->glUniform("material.emissionTex", 2);
	shader->glUniform("material.enable_emissionTex", true);

	shader->glUniform("material.shininess", _material.shininess);


	applyLightToShader(_light, *shader);
	//shader->glUniform("light.position", _light.position);
	//shader->glUniform("light.ambient", _light.ambient);
	//shader->glUniform("light.diffuse", _light.diffuse);
	//shader->glUniform("light.specular", _light.specular);
}

void LightBoxModel2::drawEnd()
{
	glBindVertexArray(0);
	glActiveTexture(0);
	glUseProgram(0);
}

