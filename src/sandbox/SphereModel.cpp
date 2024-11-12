#include "SphereModel.h"
#include "display_window_private.h"
SphereModel::SphereModel()
{
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);
	//glNamedBufferStorage(VBO, 10000 * sizeof VertexInfo, nullptr, GL_DYNAMIC_STORAGE_BIT);
	//glNamedBufferStorage(EBO, 60000 * sizeof(GLuint), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glEnableVertexArrayAttrib(VAO, 0);
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof VertexInfo);
	glVertexArrayVertexBuffer(VAO, 1, VBO, 0, sizeof VertexInfo);

	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, sizeof glm::vec3);

	glVertexArrayElementBuffer(VAO, EBO);




}

void SphereModel::initSphereVertexes(float radius, int segment)
{
	assert(segment > 2);

	const double gap = glm::pi<double>() / static_cast<float>(segment);

	vertexes.resize(static_cast<std::size_t>(segment) * (segment - 1) + 2);
	indexes.resize(static_cast<std::size_t>(segment) * (segment - 1) * 2 * 3);
	for (int row = 1; row < segment; ++row)
		for (int col = 0; col < segment; ++col)
		{
			VertexInfo& vertex = vertexes[(row - 1) * segment + col + 1];

			const double rr = gap * row;
			const double rc = gap * col * 2;
			const double lr = sin(rr);

			const double x = lr * cos(rc);
			const double y = cos(rr);
			const double z = -lr * sin(rc); // ÄæÊ±Õë·½Ïò

			vertex.location = glm::vec3{ x,y,z } *radius;
			vertex.normal = glm::vec3{ x, y, z };


			const int nc = col + 1 < segment ? col + 1 : 0;
			const int tr = row - 1;
			const int cr = row;
			const int br = row + 1;

			const int ic = (cr - 1) * segment + col + 1;
			const int in = (cr - 1) * segment + nc + 1;
			const int it = tr > 0 ? (tr - 1) * segment + col + 1 : 0;
			const int ib = br < segment ? (br - 1) * segment + nc + 1
				               : segment * (segment - 1) + 1;

			const int n = ((row - 1) * segment + col) * 6;
			indexes[n] = ic;
			indexes[n + 1] = in;
			indexes[n + 2] = it;
			indexes[n + 3] = ic;
			indexes[n + 4] = ib;
			indexes[n + 5] = in;
		}
	vertexes.front() = { glm::vec3{0,1,0} *radius,{0,1,0} };
	vertexes.back() = { glm::vec3{0,-1,0} *radius,{0,-1,0} };
}

void SphereModel::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
{
	if (!shader)
	{
		shader = ShaderProgram::makeShaderByName("SphereModel_SimpleColor.vert", "SphereModel_SimpleColor.frag");
	}
	shader->use();
	shader->glUniform("material.""diffuse", glm::vec3(0));
	shader->glUniform("material.""specular", glm::vec3(0));
	shader->glUniform("material.""emission", glm::vec3(0));
	shader->glUniform("material.""diffuseTex", 0);
	shader->glUniform("material.""specularTex", 0);
	shader->glUniform("material.""emissionTex", 0);
	if (_material.diffuse.index() == 1)
	{
		glBindTextureUnit(0, std::get<Material3::TextureID>(_material.diffuse));
		shader->glUniform("material.diffuseTex", 0);
	}
	else
	{
		glBindTextureUnit(0, 0);
		shader->glUniform("material.diffuse", std::get<Material3::Color>(_material.diffuse));
	}

	if (_material.specular.index() == 1)
	{
		glBindTextureUnit(1, std::get<Material3::TextureID>(_material.specular));
		shader->glUniform("material.specularTex", 1);
	}
	else
	{
		glBindTextureUnit(0, 0);
		shader->glUniform("material.specular", std::get<Material3::Color>(_material.specular));
	}


	if (_material.emission.index() == 1)
	{
		glBindTextureUnit(2, std::get<Material3::TextureID>(_material.emission));
		shader->glUniform("material.emissionTex", 2);
	}
	else
	{
		glBindTextureUnit(0, 0);
		shader->glUniform("material.emission", std::get<Material3::Color>(_material.emission));
	}

	shader->glUniform("material.shininess", _material.shininess);


	int i = 0;
	for (auto& light : _lights)
	{
		applyLightToShader(light, *shader, i++);
	}

	shader->glUniform("viewPos", camera.Position);
	shader->glUniform("view", camera.getViewMatrix());
	shader->glUniform("projection", camera.getProjMatrix());

	glBindVertexArray(VAO);
	for (auto& mat : wMats)
	{
		shader->glUniform("model", mat);
		glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
		auto d = WindowManagerInstance->_windowMap.begin()->second;
		switch (d->draw_mode)
		{
		case 0:glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		case 1:glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case 2:glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
		}
		//glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}
