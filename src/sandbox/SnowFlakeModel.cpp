#include "SnowFlakeModel.h"

#include <numbers>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <camera.h>
#include "DebugDraw.h"

#undef min
#undef max

SnowFlakeModel::SnowFlakeModel()
{
	if (vertexes.capacity() < iterations) vertexes.reserve(iterations);
	vertexes.emplace_back(std::vector<glm::vec3>{
		glm::vec3{0.5f, (std::numbers::inv_sqrt3_v<float> - 1) * 0.5f, 0 },
		glm::vec3{0, 0.5f, 0},
		glm::vec3{ -0.5f, (std::numbers::inv_sqrt3_v<float> -1) * 0.5f, 0 }
	});

}

SnowFlakeModel::~SnowFlakeModel()
{
	thread_alive = false;
}

void SnowFlakeModel::init()
{
	IModel::init();
	thread = std::jthread([&]()
		{
			while (thread_alive)
			{
				while (vertexes.size() < iterations)
				{
					auto ret = SnowFlakeIteration(vertexes.back());
					std::lock_guard guard(mutex);
					vertexes.emplace_back(std::move(ret));
				}
				std::this_thread::yield();
			}
		});

	glGetFloatv(GL_LINE_WIDTH, &line_width);
}

void SnowFlakeModel::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
{
	if(wMats.empty())return;
	mutex.lock();
	if(vertexes.size() > vao_s.size())
	{
		for(auto i = vao_s.size(); i < vertexes.size();++i)
		{
			unsigned int vbo;
			glCreateBuffers(1, &vbo);
			glNamedBufferData(vbo, sizeof(glm::vec3) * vertexes[i].size(), vertexes[i].data(), GL_STATIC_DRAW);

			unsigned int vao;
			glCreateVertexArrays(1, &vao);
			glEnableVertexArrayAttrib(vao, 0);
			glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(glm::vec3));
			glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

			vao_s.emplace_back(vao);
		}
	}
	mutex.unlock();
	for(size_t i = 0; i < wMats.size(); ++i)
	{
		float line_width_temp;
		glGetFloatv(GL_LINE_WIDTH, &line_width_temp);
		glLineWidth(line_width);
		for(size_t n = 0, max = std::min(vao_s.size(), iterations); n < max; ++n)
		{
			g_debug_draw->shader_single_color->use();
			g_debug_draw->shader_single_color->glUniform("projection", camera.getProjMatrix());
			g_debug_draw->shader_single_color->glUniform("view", camera.getViewMatrix());
			g_debug_draw->shader_single_color->glUniform("color", color);
			g_debug_draw->shader_single_color->glUniform("model", wMats[i] * glm::translate(glm::mat4(1), {0,0,-static_cast<float>(n)}));

			glBindVertexArray(vao_s[n]);
			glDrawArrays(GL_LINE_LOOP, 0, vertexes[n].size());
		}
		glLineWidth(line_width_temp);
	}
}

std::vector<glm::vec3> SnowFlakeModel::SnowFlakeIteration(const std::vector<glm::vec3>& polyline)
{
	// 假定 polyline 逆时针排布
	std::vector<glm::vec3> derivative;
	glm::quat quat;
	for(size_t i = 0; i < polyline.size(); ++i)
	{
		auto last = i == 0 ? polyline.back() : polyline[i - 1];
		auto start = polyline[i];
		auto end = i == (polyline.size() - 1) ? polyline.front() : polyline[i + 1];
		auto mid = (end + start) * 0.5f;
		auto off = (end - start) / 3.0f;
		if(i == 0) // 只使用构成突起顶点两边所计算的轴向
		{
			quat = glm::rotate(glm::quat(1, 0, 0, 0), glm::radians(60.0f),
				glm::normalize(glm::cross(last - start, end - start))); // 顺时针旋转60度
		}
		
		derivative.push_back(start);
		derivative.push_back(start + off);
		derivative.push_back(start + off + quat * off);
		derivative.push_back(end - off);
		
	}
	return derivative;
}
