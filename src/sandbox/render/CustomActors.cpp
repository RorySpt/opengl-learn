#include "CustomActors.h"

void MyActor::GetRandomRabbitTexture(std::vector<TextureLoader::outer_type> setters)
{
	namespace fs = std::filesystem;
	constexpr std::string_view RabbitDir = R"(C:\Users\zhang\Pictures\4K±ÚÖ½)";
	static std::vector<fs::path> rabbitPics;
	//static std::random_device rd;
	static std::default_random_engine dre(0);

	if (rabbitPics.empty())
	{
		for (auto& p : fs::directory_iterator(RabbitDir))
		{
			if (auto path = p.path(); path.has_extension())
				if (auto ext = path.extension(); ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
				{
					rabbitPics.emplace_back(p.path());
				}
		}
	}
	if (!rabbitPics.empty())
	{
		std::uniform_int_distribution uid(0, static_cast<int>(rabbitPics.size()) - 1);
		int index = uid(dre);

		std::vector<TextureLoader::input_type> inputs(setters.size());
		for (int i = 0; i < inputs.size(); ++i)
		{
			inputs[i] = TextureLoader::input_type{
				fs::path(rabbitPics[uid(dre)].c_str()),
				setters[i]
			};
		}
		g_texture_loader->loadTexture2D(inputs);
	}

}
