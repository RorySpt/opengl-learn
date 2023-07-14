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
//	else { (shader).glUniform("enable_ambientTex", true); (shader).glUniform("material.ambientTex", std::get<Material3::Maps>(material.emission)); }
//	if (std::holds_alternative<Material3::Color>(material.diffuse)) { (shader).glUniform("enable_ambientTex", false); (shader).glUniform("material.ambient", std::get<Material3::Color>(material.diffuse)); }
//	else { (shader).glUniform("enable_ambientTex", true); (shader).glUniform("material.ambientTex", std::get<Material3::Maps>(material.diffuse)); }
//	if (std::holds_alternative<Material3::Color>(material.specular)) { (shader).glUniform("enable_ambientTex", false); (shader).glUniform("material.ambient", std::get<Material3::Color>(material.specular)); }
//	else { (shader).glUniform("enable_ambientTex", true); (shader).glUniform("material.ambientTex", std::get<Material3::Maps>(material.specular)); }
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
