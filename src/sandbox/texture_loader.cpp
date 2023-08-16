#include "stdafx.h"
#include "texture_loader.h"

#include <execution>

namespace fs = std::filesystem;
TextureLoader* g_texture_loader;

TextureLoader::TextureLoader()
{
	thread = std::jthread([&]
		{
			work_thread();
		});
}

TextureLoader::~TextureLoader()
{
	bSurvival = false;
	thread.join();
}
extern std::map<std::string, unsigned int> s_textureMap;
void TextureLoader::loadTexture(std::string_view path, const outer_type& outer)
{
	loadTexture(fs::path(path), outer);
}
void TextureLoader::loadTexture(const fs::path& path, const outer_type& outer)
{
	if (s_textureMap.contains(path.string()))
	{
		outer(s_textureMap.at(path.string()));
		return;
	}

	std::lock_guard guard(raw_mutex);
	raw_task_queue.emplace(fs::absolute(path), outer);
}
void TextureLoader::loadTexture(std::vector<input_type> inputs)
{
	std::lock_guard guard(raw_mutex);
	for (auto& input : inputs)
	{
		auto path = fs::absolute(std::get<0>(input));
		auto& outer = std::get<1>(input);
		if (auto result = s_textureMap.find(path.string());
			result != s_textureMap.end())
		{
			std::get<1>(input)(result->second);
		}
		else
		{
			raw_task_queue.emplace(path, outer);
		}
	}

}
void TextureLoader::tick()
{
	while (!gen_task_queue.empty() && gen_mutex.try_lock())
	{
		auto task = gen_task_queue.front();
		gen_task_queue.pop();
		gen_mutex.unlock();

		// 检查是否有相同纹理，如果有则跳过创建
		if (auto result = s_textureMap.find(task.path.string());
			result != s_textureMap.end())
		{
			task.outer(result->second);
			continue;
		}

		GLenum format = GL_RGBA;

		texture_id_type texture_id;
		const int rw = task.intermediate_data.size.x;
		const int rh = task.intermediate_data.size.y;
		const auto& data = task.intermediate_data.data;

		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexImage2D(GL_TEXTURE_2D, 0, format, rw, rh,
			0, format, GL_UNSIGNED_BYTE, data.get());
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 返回结果
		task.outer(texture_id);

		s_textureMap.emplace(task.path.string(), texture_id);
	}


}

void TextureLoader::work_thread()
{


	while (bSurvival)
	{
		while (!raw_task_queue.empty())
		{
			std::vector<TaskInfo> tasks;
			raw_mutex.lock();
			for (int i = 0; i < max_concurrency; ++i)
			{
				if (raw_task_queue.empty())break;
				tasks.emplace_back(raw_task_queue.front());
				raw_task_queue.pop();

			}
			raw_mutex.unlock();

			int count = 0;
			std::for_each(std::execution::par, tasks.begin(), tasks.end(), [&](TaskInfo& task)
				{
					//std::cout << std::format("Load {}\n", task.path.string());
					std::cout << std::format("[{:.1f}%] Load {}\n", static_cast<double>(count++ * 100) / tasks.size(), task.path.string());

					unsigned int textureID;
					int width, height, nrComponents;


					auto& intermediate_data = task.intermediate_data;
					if (unsigned char* data = stbi_load(task.path.string().c_str(),
						&width, &height, &nrComponents, 4))
					{
						intermediate_data.data = comm::resize_image(data, width, height,
							4, intermediate_data.size.x, intermediate_data.size.y);
					}
					else
					{
						std::cout << "Texture failed to load at path: \n" << task.path.string() << std::endl;
						return;
					}
					std::lock_guard guard(gen_mutex);
					gen_task_queue.push(task);
				});

		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
