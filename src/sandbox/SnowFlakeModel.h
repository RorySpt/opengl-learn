#pragma once
#include <mutex>

#include "IModel.h"
#include <thread>
class SnowFlakeModel :
    public IModel
{
public:
	using IModel::draw;

	SnowFlakeModel();
	~SnowFlakeModel();

	void init() override;
	void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) override;

	static std::vector<glm::vec3> SnowFlakeIteration(const std::vector<glm::vec3>& polyline);

	std::vector <std::vector<glm::vec3>> vertexes;
	std::vector <unsigned int> vao_s;

	std::thread thread;
	size_t iterations = 5;
	float line_width = 1;
	bool thread_alive = true;


	glm::vec4 color = {1,0,0,1};
	std::mutex mutex;
};

