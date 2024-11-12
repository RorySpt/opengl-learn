#include "DebugDraw.h"

#include <string_view>

#include "camera.h"
#include "shader.h"

std::shared_ptr<DebugDraw> g_debug_draw;

DebugDraw::DebugDraw()
{
	init_shaders();
}

DebugDraw::~DebugDraw()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void DebugDraw::init_shaders()
{
	// 创建着色器
	constexpr std::string_view single_color_vertex_shader_code =
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
	constexpr std::string_view single_color_fragment_shader_code =
		"#version 460 core\n											 "
		"out vec4 FragColor;											 "
		"																 "
		"in vec3 TexCoords;												 "
		"																 "
		"uniform vec4 color;"
		"																 "
		"void main()													 "
		"{																 "
		"	FragColor = color;						 "
		"}																 "
		;

	shader_single_color = std::make_shared<ShaderProgram>(*ShaderBase::makeShaderByCode(single_color_vertex_shader_code.data(), ShaderBase::ST_Vert)->asVertShader()
		, *ShaderBase::makeShaderByCode(single_color_fragment_shader_code.data(), ShaderBase::ST_Frag)->asFragShader());





	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);
	glCreateBuffers(1, &ebo);
}

void DebugDraw::DrawLine(const Camera& camera, const Line& line, const Color color, const glm::mat4& trans) const
{
	DrawLines(camera, Lines{ line }, color, trans);
}

void DebugDraw::DrawLines(const Camera& camera, const Lines& lines, Color color, const glm::mat4& trans) const
{
	const int vertexes_size = lines.size() * sizeof Line;
	glNamedBufferData(vbo, vertexes_size, lines.data(), GL_DYNAMIC_DRAW);
	glVertexArrayAttribFormat(vao, 0, vertexes_size, GL_FLOAT, GL_FALSE, 0);

	shader_single_color->use();
	shader_single_color->glUniform("projection", camera.getProjMatrix());
	shader_single_color->glUniform("view", camera.getViewMatrix());
	shader_single_color->glUniform("color", color);
	shader_single_color->glUniform("model", trans);

	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, vertexes_size / sizeof Point);
}

void DebugDraw::DrawLines(const Camera& camera, const Lines& lines, Colors colors, const glm::mat4& trans) const
{
	if (colors.empty()) colors.emplace_back( 0,0,0,1 );

	const int vertexes_size = lines.size() * sizeof Line;
	glNamedBufferData(vbo, vertexes_size, lines.data(), GL_DYNAMIC_DRAW);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

	shader_single_color->use();
	shader_single_color->glUniform("projection", camera.getProjMatrix());
	shader_single_color->glUniform("view", camera.getViewMatrix());
	
	shader_single_color->glUniform("model", trans);

	glBindVertexArray(vao);
	size_t line_index = 0;
	size_t color_index = 0;
	for(; color_index < colors.size() && line_index < lines.size(); ++color_index, ++line_index)
	{
		shader_single_color->glUniform("color", colors[color_index]);
		glDrawArrays(GL_LINES, static_cast<int>(line_index * 2), 2);
	}
	if(line_index != lines.size())
	{
		for (; line_index < lines.size(); ++line_index)
		{
			shader_single_color->glUniform("color", colors.back());
			glDrawArrays(GL_LINES, static_cast<int>(line_index * 2), 2);
		}
	}
}

void DebugDraw::DrawPolyline(const Camera& camera, const Polyline& lines, Color color, bool is_close, const glm::mat4& trans) const
{
	const int vertexes_size = lines.size() * sizeof Point;
	glNamedBufferData(vbo, vertexes_size, lines.data(), GL_DYNAMIC_DRAW);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

	shader_single_color->use();
	shader_single_color->glUniform("projection", camera.getProjMatrix());
	shader_single_color->glUniform("view", camera.getViewMatrix());
	shader_single_color->glUniform("color", color);
	shader_single_color->glUniform("model", trans);

	glBindVertexArray(vao);
	glDrawArrays(is_close ? GL_LINE_LOOP : GL_LINE_STRIP, 0, lines.size());
}

void DebugDraw::DrawPolyline(const Camera& camera, const Polylines& lines, Colors colors, bool is_close, const glm::mat4& trans) const
{
	if (colors.empty()) colors.emplace_back(0, 0, 0, 1);

	size_t line_index = 0;
	size_t color_index = 0;
	for (; color_index < colors.size() && line_index < lines.size(); ++color_index, ++line_index)
	{
		DrawPolyline(camera, lines[line_index], colors[color_index], is_close, trans);
	}
	if (line_index != lines.size())
	{
		for (; line_index < lines.size(); ++line_index)
		{
			DrawPolyline(camera, lines[line_index], colors.back(), is_close, trans);
		}
	}
}

void DebugDraw::DrawBox(const Camera& camera, const Box& box, const Color color, const glm::mat4& trans) const
{
	auto [extent, center] = static_cast<ExtentBox>(box);

	Points points;
	points.emplace_back(center + glm::vec3{ -extent.x,-extent.y, -extent.z });
	points.emplace_back(center + glm::vec3{ -extent.x,-extent.y, extent.z });
	points.emplace_back(center + glm::vec3{ extent.x,-extent.y, extent.z });
	points.emplace_back(center + glm::vec3{ extent.x,-extent.y, -extent.z });
	points.emplace_back(center + glm::vec3{ -extent.x,extent.y, -extent.z });
	points.emplace_back(center + glm::vec3{ -extent.x,extent.y, extent.z });
	points.emplace_back(center + glm::vec3{ extent.x,extent.y, extent.z });
	points.emplace_back(center + glm::vec3{ extent.x,extent.y, -extent.z });

	constexpr std::array indexes = {
		0U,1U,
		1U,2U,
		2U,3U,
		3U,0U,
		4U,5U,
		5U,6U,
		6U,7U,
		7U,4U,
		0U,4U,
		1U,5U,
		2U,6U,
		3U,7U,
	};

		

	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof Point); // 元素起始为0， 元素间隔为 Point
	

	glNamedBufferData(vbo, points.size() * sizeof Points, points.data(), GL_DYNAMIC_DRAW);  // NOLINT(bugprone-sizeof-container)
	glNamedBufferData(ebo, sizeof indexes, indexes.data(), GL_DYNAMIC_DRAW);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, 0);
	glVertexArrayElementBuffer(vao, ebo);

	shader_single_color->use();
	shader_single_color->glUniform("projection", camera.getProjMatrix());
	shader_single_color->glUniform("view", camera.getViewMatrix());
	shader_single_color->glUniform("color", color);
	shader_single_color->glUniform("model", trans);



	glBindVertexArray(vao);
	
	glDrawElements(GL_LINES, indexes.size(), GL_UNSIGNED_INT, nullptr);
}
