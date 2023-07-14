#pragma once
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include "common.h"
#include <variant>

#include "shader.h"

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};
struct Material2 {
	unsigned int emissionTex;	// 自发光贴图
	unsigned int diffuseTex;	// 漫反射贴图
	unsigned int specularTex;	// 镜面光贴图
	float shininess;
};

struct Material3
{
	using Color = glm::vec3;
	using Maps = unsigned int;
	using ColorOrMaps = std::variant<Color, Maps>;


	ColorOrMaps emission;
	ColorOrMaps diffuse;
	ColorOrMaps specular;
	float shininess{};
};
//void applyMaterial3ToShader(const Material3& material, const ShaderProgram& shader,std::array<int,3> loc = {0,1,2});




// Design Patterns: Singleton
class MaterialTable
{
public:
	using TableMap = std::map<std::string, Material>;
	static MaterialTable* instance();

	bool insert_or_assign(const std::string&, const Material&);
	
	const Material& getMaterial(const std::string& name) const;
	[[nodiscard]] TableMap::const_iterator findMaterial(const std::string& name)const;
	std::vector<Material> getMaterials();
	std::vector<std::string> getMaterialNames();
private:
	MaterialTable();


	std::map<std::string, Material> m_map;
};