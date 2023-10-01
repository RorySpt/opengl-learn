#pragma once
#include "IModel.h"
#include "common.h"
#include <mutex>
#include <thread>

class SnowFlakeModel :
    public IModel
{
public:
	CLASS_DEFAULT_COPY_AND_MOVE(SnowFlakeModel)
	using IModel::draw;

	SnowFlakeModel();
	~SnowFlakeModel() override;

	void init() override;
	void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) override;

	static std::vector<glm::vec3> SnowFlakeIteration(const std::vector<glm::vec3>& polyline);

	std::vector <std::vector<glm::vec3>> vertexes;
	std::vector <unsigned int> vao_s;

	std::jthread thread;
	size_t iterations = 5;
	float line_width = 1;
	bool thread_alive = true;


	glm::vec4 color = {1,0,0,1};
	std::mutex mutex;
};

