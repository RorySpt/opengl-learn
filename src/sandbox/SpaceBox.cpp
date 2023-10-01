#include "stdafx.h"
#include "SpaceBox.h"
#include "texture_loader.h"
#include "nlohmann/json.hpp"
#include "configure.h"
#include <fstream>

#include "camera.h"
#include "shader.h"
#include <GL/gl.h>

std::shared_ptr<ShaderProgram>  shader_program;
void SpaceBox::init()
{
	IModel::init();
	const std::array load_inputs = {
			std::array<std::string_view,2>{ PROJECT_DATA_PATH, "/picture/skybox/right.jpg" } | std::views::join | std::ranges::to<std::string>(),
		std::array<std::string_view,2>{ PROJECT_DATA_PATH, "/picture/skybox/left.jpg" } | std::views::join | std::ranges::to<std::string>(),
		std::array<std::string_view,2>{ PROJECT_DATA_PATH, "/picture/skybox/top.jpg" } | std::views::join | std::ranges::to<std::string>(),
		std::array<std::string_view,2>{ PROJECT_DATA_PATH, "/picture/skybox/bottom.jpg" } | std::views::join | std::ranges::to<std::string>(),
		std::array<std::string_view,2>{ PROJECT_DATA_PATH, "/picture/skybox/front.jpg" } | std::views::join | std::ranges::to<std::string>(),
		std::array<std::string_view,2>{ PROJECT_DATA_PATH, "/picture/skybox/back.jpg"} | std::views::join | std::ranges::to<std::string>(),

	};
	
	//GL_TEXTURE_CUBE_MAP_POSITIVE_X

	const std::array faces_index = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	// 创建并初始化CubeSample
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture_id);
	glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	int width_unite = 0, height_unite = 0;
	for(int i = 0; i < 6; ++i)
	{
		auto &path = load_inputs[i];//load_inputs[0];
		int width, height, nrComponents;
		constexpr int desired_channels = 4;
		std::shared_ptr<stbi_uc[]> data;
		if((data = std::shared_ptr<stbi_uc[]>(stbi_load(path.c_str(), &width, &height, &nrComponents, desired_channels))) )
		{
			if(i == 0)
			{
				data = comm::resize_image(data.get(), width, height,
					desired_channels, width, height);
				glTextureStorage2D(texture_id, 8, GL_RGBA8, width, width);
				width_unite = width;
				height_unite = height;
			}else
			{
				data = comm::resize_image(data.get(), width, height,
					desired_channels, width_unite);
			}
		}
		glTextureSubImage3D(texture_id, 0,0,0, i,width_unite,height_unite,1, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
	}

	glGenerateTextureMipmap(texture_id);
	constexpr std::array vertexes = {
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
	};
	constexpr std::array<unsigned, 36> indexes = {

		/// 右
		0,1,7,
		7,4,0,

		/// 左
		5,6,2,
		2,3,5,

		/// 上
		0,4,5,
		5,3,0,

		/// 下
		6,7,1,
		1,2,6,

		/// 后
		4,7,6,
		6,5,4,
		/// 前
		0,3,2,
		2,1,0,
		
	};

	// 创建并初始化缓存对象
	unsigned int buf;
	glCreateBuffers(1, &buf);
	glNamedBufferStorage(buf, sizeof vertexes, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(buf, 0, sizeof vertexes, vertexes.data());

	glCreateBuffers(1, &EBO);
	glNamedBufferStorage(EBO, sizeof indexes, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(EBO, 0, sizeof indexes, indexes.data());
	// 创建并初始化顶点属性
	glCreateVertexArrays(1, &VAO);
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayVertexBuffer(VAO, 0, buf, 0, sizeof(float) * 3);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, false, 0);
	glVertexArrayElementBuffer(VAO, EBO);


	// 创建着色器
	constexpr std::string_view vertex_shader_code =
		"#version 460 core\n											 "
		"layout(location = 0) in vec3 aPos;								 "
		"																 "
		"out vec3 TexCoords;											 "
		"																 "
		"uniform mat4 projection;										 "
		"uniform mat4 view;												 "
		"uniform mat4 model;											 "

		"void main()													 "
		"{																 "
		"	TexCoords = aPos;											 "
		"	gl_Position = projection * view * model * vec4(aPos, 1.0);	 "
		"}																 "
		;
	constexpr std::string_view fragment_shader_code =
		"#version 460 core\n											 "
		"out vec4 FragColor;											 "
		"																 "
		"in vec3 TexCoords;												 "
		"																 "
		"uniform samplerCube skybox;									 "
		"																 "
		"void main()													 "
		"{																 "
		"	FragColor = texture(skybox, TexCoords);						 "
		"}																 "
		;

	shader_program = std::make_shared<ShaderProgram>(*ShaderBase::makeShaderByCode(vertex_shader_code.data(), ShaderBase::ST_Vert)->asVertShader()
		, *ShaderBase::makeShaderByCode(fragment_shader_code.data(), ShaderBase::ST_Frag)->asFragShader());
}

void SpaceBox::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
{
	shader_program->use();
	for(const auto mat:wMats)
	{
		shader_program->glUniform("projection", camera.getProjMatrix());
		shader_program->glUniform("view", camera.getViewMatrix());
		shader_program->glUniform("model", mat);
		glBindTextureUnit(0, texture_id);
		glBindVertexArray(VAO);
		//glDepthMask(GL_FALSE);
		//glDrawArrays(GL_TRIANGLES, 0,108);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

		//glDepthMask(GL_TRUE);
	}
}
