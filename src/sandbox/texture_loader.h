#pragma once
#include <filesystem>
#include <queue>

#include"common.h"

/**
 * \brief �������� ���� OpenglAPI û�취���������ã���˽� opengl ��������������ݷ���tick�����У���Ҫ�����߳�ѭ���е���
 */
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
		texture_id_type id;
	};

	TextureLoader(GLFWwindow* sharedWindow);
	~TextureLoader();

	void loadTexture2D(std::string_view path, const outer_type&);
	void loadTexture2D(const std::filesystem::path& path, const outer_type& id_outer);
	void loadTexture2D(std::vector<input_type> inputs);

	void tick(); // ���߳�


private:
	void work_thread(); // ���߳�
	void work_thread2(); // ���̣߳�ӵ�ж�����Opengl������


	bool bSurvival = true;

	int max_concurrency = std::thread::hardware_concurrency() > 3 ? static_cast<int>(std::thread::hardware_concurrency()) / 2: 1;

	std::mutex raw_mutex, gen_mutex, res_mutex;
	std::queue<TaskInfo> raw_task_queue;
	std::queue<TaskInfo> gen_task_queue;
	std::queue<TaskInfo> res_task_queue;
	std::jthread thread;
	std::jthread thread2;

	GLFWwindow* _sharedWindow;
	GLFWwindow* _threadContext;
};


extern TextureLoader* g_texture_loader;