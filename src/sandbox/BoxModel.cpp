#include "stdafx.h"
#include "BoxModel.h"
#include "common.h"
#include "vertices.h"
#include "camera.h"
#include "shader.h"
BoxModel_SingleColor::BoxModel_SingleColor()
{
	
	init();
}

BoxModel_SingleColor::~BoxModel_SingleColor()
{
	finished();
}

void BoxModel_SingleColor::init()
{
	shader = ShaderProgram::makeShaderByName("PureColorMaterial.vert","PureColorMaterial.frag");
	//shader->use();
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

void BoxModel_SingleColor::finished()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void BoxModel_SingleColor::draw(const Camera& camera, const glm::mat4& wMat)
{
	drawBegin();
	shader->glUniform("viewPos", camera.Position);
	shader->glUniform("view", camera.getViewMatrix());
	shader->glUniform("projection", camera.getProjMatrix());
	shader->glUniform("model", wMat);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	drawEnd();
}

void BoxModel_SingleColor::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
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

void BoxModel_SingleColor::drawBegin()
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

void BoxModel_SingleColor::drawEnd()
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
	return comm::loadTexture(std::string(comm::dir_picture) + "/01.jpeg");
}
BoxModel_SimpleTexture::BoxModel_SimpleTexture()
{
	
	initBuffers();
	initTextures();

	shader = ShaderProgram::makeShaderByName("common.vert", "common.frag");
}

BoxModel_SimpleTexture::~BoxModel_SimpleTexture()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void BoxModel_SimpleTexture::draw(const Camera& camera, const glm::mat4& wMat)
{
	drawBegin();
	shader->glUniform("viewPos", camera.Position);
	shader->glUniform("view", camera.getViewMatrix());
	shader->glUniform("projection", camera.getProjMatrix());
	shader->glUniform("model", wMat);

	//glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	drawEnd();
}

void BoxModel_SimpleTexture::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
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

void BoxModel_SimpleTexture::drawBegin()
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

void BoxModel_SimpleTexture::drawEnd()
{
	glBindVertexArray(0);
	glActiveTexture(0);
	glUseProgram(0);
}

void BoxModel_SimpleTexture::initBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	struct vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 coordinate;
	};
	using triangle = std::array<vertex, 3>;

	using face = std::array<vertex, 6>;


	face faces[6];
	
	memcpy(&faces, vertices2, sizeof face * 6);

	auto pi = glm::pi<float>();
	auto pi_2 = pi / 2;

	glm::mat4 mat_back =
		glm::rotate(glm::mat4{1}, pi, { 0,1,0 });

	glm::mat4 mat_left =
		 glm::rotate(glm::mat4{1}, -pi_2, { 0,1,0 });

	glm::mat4 mat_right =
		 glm::rotate(glm::mat4{1}, pi_2, { 0,1,0 });

	glm::mat4 mat_top =
		 glm::rotate(glm::mat4{1}, -pi_2, { 1,0,0 });
	glm::mat4 mat_bottom =
		 glm::rotate(glm::mat4{1}, pi_2, { 1,0,0 });

	auto transformFace = [&f = faces[1]](face& face, glm::mat4 mat)
	{

		for(int i = 0; i < face.size(); ++i)
		{
			face[i].position = mat * glm::vec4{ f[i].position, 1 };
			face[i].coordinate = f[i].coordinate;
			face[i].normal = mat * glm::vec4{ f[i].normal, 1 };

		}
	};
	transformFace(faces[0], mat_back);
	transformFace(faces[2], mat_left);
	transformFace(faces[3], mat_right);
	transformFace(faces[4], mat_top);
	transformFace(faces[5], mat_bottom);

	auto printFace = [](const face& face_)
	{
		for (auto& ver : face_)
		{
			std::cout << std::format("{: .1f}, {: .1f}, {: .1f}, {: .1f}, {: .1f}, {: .1f}, {: .1f}, {: .1f},\n"
				, ver.position[0]
				, ver.position[1]
				, ver.position[2]
				, ver.normal[0]
				, ver.normal[1]
				, ver.normal[2]
				, ver.coordinate[0]
				, ver.coordinate[1]
			);
		}
		std::cout << '\n';
	};
	for(const auto& face:faces)
	{
		printFace(face);
	}


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof face * 6, &vertices3, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));  // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));  // NOLINT(performance-no-int-to-ptr)
	glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void BoxModel_SimpleTexture::initTextures()
{

	auto container2_diffuse = comm::loadTexture(std::string(comm::dir_picture) + "/container2.png");
	auto container2_specular = comm::loadTexture(std::string(comm::dir_picture) + "/container2_specular.png");

	setMaterial({ GetRandomRabbitTexture(),
		container2_diffuse
		,container2_specular
		,32.0f });
}

