#pragma once
#include <array>
#include <vector>
#include <glm/glm.hpp>

#include "shader.h"


class Camera;

class DebugDraw
{
public:
	using Point = glm::vec3;
	using Points = std::vector<Point>;
	using Line = std::array<glm::vec3, 2>;
	using Lines = std::vector<Line>;
	using Polyline = std::vector<Point>;
	using Polylines = std::vector<Polyline>;
	using Triangle = std::array<glm::vec3, 3>;
	using Primitive = std::vector<Triangle>;
	using Color = glm::vec4;
	using Colors = std::vector<Color>;

	struct ExtentBox;
	struct Box
	{
		Point Min;
		Point Max;

		operator ExtentBox() const { return { (Max - Min) * 0.5f, (Max + Min) * 0.5f }; }
	};
	struct ExtentBox
	{
		glm::vec3 extent = { 0.5f,0.5f,0.5f };
		glm::vec3 center = {0,0,0};
		operator Box() const { return { center + extent, center - extent }; }
	};

	CLASS_DEFAULT_COPY_AND_MOVE(DebugDraw)

	DebugDraw();
	~DebugDraw();

	
	void DrawLine(const Camera& camera, const Line& line, Color color, const glm::mat4& trans = glm::mat4(1)) const;
	void DrawLines(const Camera& camera, const Lines& lines, Color color, const glm::mat4& trans = glm::mat4(1)) const;
	void DrawLines(const Camera& camera, const Lines& lines, Colors colors, const glm::mat4& trans = glm::mat4(1)) const;
	void DrawPolyline(const Camera& camera, const Polyline& lines, Color color, bool is_close = false, const glm::mat4& trans = glm::mat4(1)) const;
	void DrawPolyline(const Camera& camera, const Polylines& lines, Colors colors, bool is_close = false, const glm::mat4& trans = glm::mat4(1)) const;

	void DrawBox(const Camera& camera, const Box& box, Color color, const glm::mat4& trans = glm::mat4(1)) const;


	std::shared_ptr<ShaderProgram>  shader_single_color;
private:
	void init_shaders();

	
	unsigned vao, vbo, ebo;
};

extern std::shared_ptr<DebugDraw> g_debug_draw;