#pragma once
#include <filesystem>
#include <queue>

#include"common.h"

class TextureLoader final
{
public:
	using texture_id_type = unsigned int;
	using outer_type = std::function<void(texture_id_type)>;
	using input_type = std::tuple<std::filesystem::path, outer_type>;

	CLASS_NO_COPY_AND_MOVE(TextureLoader)

	struct IntermediateData
	{
		std::shared_ptr<stbi_uc[]> data;
		glm::i32vec2 size;
	};
	struct TaskInfo
	{
		std::filesystem::path path;
		outer_type outer;
		IntermediateData intermediate_data;
	};

	TextureLoader();
	~TextureLoader();

	void loadTexture(std::string_view path, const outer_type&);
	void loadTexture(const std::filesystem::path& path, const outer_type& id_outer);
	void loadTexture(std::vector<input_type> inputs);

	void tick(); // 主线程


private:
	void work_thread(); // 子线程



	bool bSurvival = true;

	int max_concurrency = std::numeric_limits<int>::max();

	std::mutex raw_mutex, gen_mutex;
	std::queue<TaskInfo> raw_task_queue;
	std::queue<TaskInfo> gen_task_queue;
	std::jthread thread;
};


extern TextureLoader* g_texture_loader;