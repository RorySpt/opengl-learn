#include "stdafx.h"
#include "material.h"

#include <fstream>
#include <ranges>
#include <regex>
#include <sstream>


//void applyMaterial3ToShader(const Material3& material, const ShaderProgram& shader)
//{
//	if (std::holds_alternative<Material3::Color>(material.emission))
//	{
//		(shader).glUniform("enable_ambientTex", false); (shader).glUniform("material.ambient", std::get<Material3::Color>(material.emission));
//	}
//	else { (shader).glUniform("enable_ambientTex", true); (shader).glUniform("material.ambientTex", std::get<Material3::TextureID>(material.emission)); }
//	if (std::holds_alternative<Material3::Color>(material.diffuse)) { (shader).glUniform("enable_ambientTex", false); (shader).glUniform("material.ambient", std::get<Material3::Color>(material.diffuse)); }
//	else { (shader).glUniform("enable_ambientTex", true); (shader).glUniform("material.ambientTex", std::get<Material3::TextureID>(material.diffuse)); }
//	if (std::holds_alternative<Material3::Color>(material.specular)) { (shader).glUniform("enable_ambientTex", false); (shader).glUniform("material.ambient", std::get<Material3::Color>(material.specular)); }
//	else { (shader).glUniform("enable_ambientTex", true); (shader).glUniform("material.ambientTex", std::get<Material3::TextureID>(material.specular)); }
//	shader.glUniform("material.shininess", material.shininess);
//}

MaterialTable* MaterialTable::instance()
{
	static MaterialTable* instacne = nullptr;
	if(!instacne)
	{
		instacne = new MaterialTable;
	}
	return instacne;
}

bool MaterialTable::insert_or_assign(const std::string& name, const Material& material)
{
	return m_map.insert_or_assign(name, material).second;
}


const Material& MaterialTable::getMaterial(const std::string& name) const
{
	return m_map.at(name);
}

std::map<std::string, Material>::const_iterator MaterialTable::findMaterial(const std::string& name) const
{
	return m_map.find(name);
}

std::vector<Material> MaterialTable::getMaterials()
{
	auto a = m_map | std::views::values;
	return {a.begin(),a.end()};
}

std::vector<std::string> MaterialTable::getMaterialNames()
{
	auto a = m_map | std::views::keys;
	return { a.begin(),a.end() };
}

MaterialTable::MaterialTable()
{

	std::ifstream ifstream(comm::path_materials.data());
	std::cout << std::format("begin to load: {}\n", comm::path_materials.data());
	if(ifstream.good())
	{
		std::stringstream stringstream;
		stringstream << ifstream.rdbuf();
		std::string materials = stringstream.str();

		std::regex regex(R"(([a-zA-Z][a-zA-Z ]*[a-zA-Z])(.*))");
		std::regex re_val(R"(([0-9][0-9\.]*))");
		
		try
		{
			std::sregex_iterator words_begin = { materials.begin(),materials.end(),regex };
			auto words_end = std::sregex_iterator();
			for(auto i = words_begin;i!= words_end;++i)
			{
				std::smatch smatch = *i;
				if(smatch.size() == 3)
				{
					Material& material = m_map[smatch[1]];
					const std::string& params = smatch[2];
					words_begin = { params.begin(),params.end(),re_val };
					int n = 0;
					for (auto j = words_begin; j != words_end; ++j)
					{
						std::string val = j->str();
						if (n < 10)std::from_chars(val.c_str(), val.c_str() + val.size(), *(&material.ambient.x + n++));
						else std::cout << std::format("format may not be correct: {}\n", smatch.str());
					}
					material.shininess *= 128;
				}else
				{
					std::cout << std::format("match failed at {}\n", smatch.str());
				}
				
			}
			std::cout << std::format("load successfully!\n");
		}catch (std::exception& e)
		{
			std::cout << e.what() << '\n';
		}
		


	}else
	{
		std::cout << std::format("error to load {}\n", comm::path_materials);
	}
	
	insert_or_assign("default", { {1.0f, 0.5f, 0.31f},{1.0f, 0.5f, 0.31f},{0.5f, 0.5f, 0.5f},32.0f });
}

namespace Materials {

	// ´äÂÌÉ«
	Material emerald = {
		glm::vec3(0.0215f, 0.1745f, 0.0215f),
		glm::vec3(0.07568f, 0.61424f, 0.07568f),
		glm::vec3(0.633f, 0.727811f, 0.633f),
		0.6f
	};

	// ÓñÊ¯ÂÌ
	Material jade = {
		glm::vec3(0.135f, 0.2225f, 0.1575f),
		glm::vec3(0.54f, 0.89f, 0.63f),
		glm::vec3(0.316228f, 0.316228f, 0.316228f),
		0.1f
	};

	// ºÚê×Ê¯
	Material obsidian = {
		glm::vec3(0.05375f, 0.05f, 0.06625f),
		glm::vec3(0.18275f, 0.17f, 0.22525f),
		glm::vec3(0.332741f, 0.328634f, 0.346435f),
		0.3f
	};

	// ÕäÖé°×
	Material pearl = {
		glm::vec3(0.25f, 0.20725f, 0.20725f),
		glm::vec3(1.0f, 0.829f, 0.829f),
		glm::vec3(0.296648f, 0.296648f, 0.296648f),
		0.088f
	};

	// ºì±¦Ê¯
	Material ruby = {
		glm::vec3(0.1745f, 0.01175f, 0.01175f),
		glm::vec3(0.61424f, 0.04136f, 0.04136f),
		glm::vec3(0.727811f, 0.626959f, 0.626959f),
		0.6f
	};

	// ÂÌËÉÊ¯
	Material turquoise = {
		glm::vec3(0.1f, 0.18725f, 0.1745f),
		glm::vec3(0.396f, 0.74151f, 0.69102f),
		glm::vec3(0.297254f, 0.30829f, 0.306678f),
		0.1f
	};

	// »ÆÍ­
	Material brass = {
		glm::vec3(0.329412f, 0.223529f, 0.027451f),
		glm::vec3(0.780392f, 0.568627f, 0.113725f),
		glm::vec3(0.992157f, 0.941176f, 0.807843f),
		0.21794872f
	};

	// ÇàÍ­
	Material bronze = {
		glm::vec3(0.2125f, 0.1275f, 0.054f),
		glm::vec3(0.714f, 0.4284f, 0.18144f),
		glm::vec3(0.393548f, 0.271906f, 0.166721f),
		0.2f
	};

	// ¸õ
	Material chrome = {
		glm::vec3(0.25f, 0.25f, 0.25f),
		glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(0.774597f, 0.774597f, 0.774597f),
		0.6f
	};

	// Í­
	Material copper = {
		glm::vec3(0.19125f, 0.0735f, 0.0225f),
		glm::vec3(0.7038f, 0.27048f, 0.0828f),
		glm::vec3(0.256777f, 0.137622f, 0.086014f),
		0.1f
	};

	// »Æ½ð
	Material gold = {
		glm::vec3(0.24725f, 0.1995f, 0.0745f),
		glm::vec3(0.75164f, 0.60648f, 0.22648f),
		glm::vec3(0.628281f, 0.555802f, 0.366065f),
		0.4f
	};

	// Òø
	Material silver = {
		glm::vec3(0.19225f, 0.19225f, 0.19225f),
		glm::vec3(0.50754f, 0.50754f, 0.50754f),
		glm::vec3(0.508273f, 0.508273f, 0.508273f),
		0.4f
	};

	// ºÚÉ«ËÜÁÏ
	Material black_plastic = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.01f, 0.01f, 0.01f),
		glm::vec3(0.50f, 0.50f, 0.50f),
		0.25f
	};

	// ÇàÉ«ËÜÁÏ
	Material cyan_plastic = {
		glm::vec3(0.0f, 0.1f, 0.06f),
		glm::vec3(0.0f, 0.50980392f, 0.50980392f),
		glm::vec3(0.50196078f, 0.50196078f, 0.50196078f),
		0.25f
	};

	// ÂÌÉ«ËÜÁÏ
	Material green_plastic = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.1f, 0.35f, 0.1f),
		glm::vec3(0.45f, 0.55f, 0.45f),
		0.25f
	};

	// ºìÉ«ËÜÁÏ
	Material red_plastic = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.5f, 0.0f, 0.0f),
		glm::vec3(0.7f, 0.6f, 0.6f),
		0.25f
	};

	// °×É«ËÜÁÏ
	Material white_plastic = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.55f, 0.55f, 0.55f),
		glm::vec3(0.70f, 0.70f, 0.70f),
		0.25f
	};

	// »ÆÉ«ËÜÁÏ
	Material yellow_plastic = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.0f),
		glm::vec3(0.60f, 0.60f, 0.50f),
		0.25f
	};

	// ºÚÉ«ÏðÆ¤
	Material black_rubber = {
		glm::vec3(0.02f, 0.02f, 0.02f),
		glm::vec3(0.01f, 0.01f, 0.01f),
		glm::vec3(0.4f, 0.4f, 0.4f),
		0.078125f
	};

	// ÇàÉ«ÏðÆ¤
	Material cyan_rubber = {
		glm::vec3(0.0f, 0.05f, 0.05f),
		glm::vec3(0.4f, 0.5f, 0.5f),
		glm::vec3(0.04f, 0.7f, 0.7f),
		0.078125f
	};

	// ÂÌÉ«ÏðÆ¤
	Material green_rubber = {
		glm::vec3(0.0f, 0.05f, 0.0f),
		glm::vec3(0.4f, 0.5f, 0.4f),
		glm::vec3(0.04f, 0.7f, 0.04f),
		0.078125f
	};

	// ºìÉ«ÏðÆ¤
	Material red_rubber = {
		glm::vec3(0.05f, 0.0f, 0.0f),
		glm::vec3(0.5f, 0.4f, 0.4f),
		glm::vec3(0.7f, 0.04f, 0.04f),
		0.078125f
	};

	// °×É«ÏðÆ¤
	Material white_rubber = {
		glm::vec3(0.05f, 0.05f, 0.05f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.7f, 0.7f, 0.7f),
		0.078125f
	};

	// »ÆÉ«ÏðÆ¤
	Material yellow_rubber = {
		glm::vec3(0.05f, 0.05f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.4f),
		glm::vec3(0.7f, 0.7f, 0.04f),
		0.078125f
	};
}