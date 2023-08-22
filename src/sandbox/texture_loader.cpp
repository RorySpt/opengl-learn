#include "stdafx.h"
#include "texture_loader.h"

#include <execution>

namespace fs = std::filesystem;
TextureLoader* g_texture_loader;

TextureLoader::TextureLoader(GLFWwindow* sharedWindow)
	:_sharedWindow(sharedWindow)
{
	glfwWindowHint(GLFW_VISIBLE, false);
	_threadContext = glfwCreateWindow(1, 1, "", nullptr, sharedWindow);
	glfwHideWindow(_threadContext);
	thread = std::jthread([&]
		{
			work_thread();
			return 0;
		});
	thread2 = std::jthread([&]
		{
			work_thread2();
			return 0;
		});
}

TextureLoader::~TextureLoader()
{
	bSurvival = false;
	thread.join();
	thread2.join();
	glfwDestroyWindow(_threadContext);
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
	while (!res_task_queue.empty() && res_mutex.try_lock())
	{
		auto task = res_task_queue.front();
		res_task_queue.pop();
		res_mutex.unlock();

		// 检查是否有相同纹理，如果有则跳过创建
		if (auto result = s_textureMap.find(task.path.string());
			result != s_textureMap.end())
		{

			//glDeleteTextures(1, &task.id);
			task.outer(result->second);
			return;
		}
		

		
		//texture_id_type texture_id;
		//const int rw = task.intermediate_data.size.x;
		//const int rh = task.intermediate_data.size.y;
		//const auto& data = task.intermediate_data.data;
		

		// 返回结果
		task.outer(task.id);

		s_textureMap.emplace(task.path.string(), task.id);
	}


}

void TextureLoader::work_thread()
{

	
	while (bSurvival)
	{
		while (!raw_task_queue.empty()&& raw_mutex.try_lock())
		{
			std::vector<TaskInfo> temps;
			std::vector<TaskInfo> tasks;

			
			while(!raw_task_queue.empty())
			{

				temps.emplace_back(raw_task_queue.front());
				raw_task_queue.pop();
			}
			raw_mutex.unlock();

			std::chrono::duration<double> d0, w, d1;

			auto tp = std::chrono::high_resolution_clock::now();
			// 去重
			std::ranges::sort(temps, [](const TaskInfo& a, const TaskInfo& b)
				{
					return a.path < b.path;
				});
			tasks.emplace_back(temps[0]);
			for(int i = 1, last = 0;i< temps.size();++i)
			{
				if(temps[i].path != temps[last].path)
				{
					tasks.emplace_back(temps[i]);
					last = i;
				}
			}
			d0 = std::chrono::high_resolution_clock::now() - tp;

			tp = std::chrono::high_resolution_clock::now();
			int count = 0;
			for(int i = 0; i < tasks.size(); i += max_concurrency)
			{
				const auto begin = tasks.begin() + i;
				const auto end = i + max_concurrency < tasks.size()
											? tasks.begin() + i + max_concurrency : tasks.end();

				std::for_each(std::execution::par, begin, end, [&](TaskInfo& task)
					{
						
						unsigned int textureID;
						int width, height, nrComponents;
						constexpr int desired_channels = 4;

						auto& intermediate_data = task.intermediate_data;
						if (unsigned char* data = stbi_load(task.path.string().c_str(),
							&width, &height, &nrComponents, desired_channels))
						{
							if (!bSurvival) return; // 中断立即返回
							intermediate_data.data = comm::resize_image(data, width, height,
								desired_channels, intermediate_data.size.x, intermediate_data.size.y);
						}
						else
						{
							std::cout << "Texture failed to load at path: \n" << task.path.string() << std::endl;
							return;
						}

						//std::cout << std::format("Load {}\n", task.path.string());
						std::cout << std::format("[{:.1f}%] Load {}\n", static_cast<double>(count++ * 100) / tasks.size(), task.path.string());

					});
				if (!bSurvival) return;
			}
			w = std::chrono::high_resolution_clock::now() - tp;

			tp = std::chrono::high_resolution_clock::now();
			// 进行赋值
			for (int i = 0, n = 0; i < temps.size(); ++i)
			{
				if (temps[i].path == tasks[n].path)
				{
					temps[i].intermediate_data = tasks[n].intermediate_data;
				}else
				{
					temps[i].intermediate_data = tasks[++n].intermediate_data;
				}
			}
			d1 = std::chrono::high_resolution_clock::now() - tp;

			std::cout << std::format("Cast: d0 = {} sec，w = {} sec，d1 = {} sec\n", d0, w, d1);

			std::lock_guard guard(gen_mutex);
			for(auto task:temps)
				gen_task_queue.push(task);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void TextureLoader::work_thread2()
{
	glfwMakeContextCurrent(_threadContext);

	while (bSurvival)
	{
		
		while (!gen_task_queue.empty()&& gen_mutex.try_lock())
		{
			std::vector<TaskInfo> tasks;
			while(!gen_task_queue.empty())
			{
				tasks.emplace_back(gen_task_queue.front());
				gen_task_queue.pop();
			}
			gen_mutex.unlock();

			//std::vector<texture_id_type> ids(tasks.size());
			//glCreateTextures(GL_TEXTURE_2D, ids.size(), ids.data());

			for (int i = 0; i < tasks.size(); ++i)
			{
				texture_id_type texture_id;
				glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
				auto& task = tasks[i];

				const int rw = task.intermediate_data.size.x;
				const int rh = task.intermediate_data.size.y;
				const auto& data = task.intermediate_data.data;

				glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTextureStorage2D(texture_id, 8, GL_RGBA8, rw, rh);
				glTextureSubImage2D(texture_id, 0, 0, 0, rw, rh, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
				glGenerateTextureMipmap(texture_id);

				glFlush();
				task.id = texture_id;
				
				{
					std::lock_guard guard(res_mutex);
					res_task_queue.push(task);
				}
				
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
			glFlush();

			//res_mutex.lock();
			//for (auto& task : tasks)
			//	res_task_queue.push(task);
			//res_mutex.unlock();


		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
