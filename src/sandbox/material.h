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
	using TextureID = unsigned int;
	using ColorOrTexture = std::variant<Color, TextureID>;


	ColorOrTexture emission;
	ColorOrTexture diffuse;
	ColorOrTexture specular;
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

namespace Materials {
	extern Material emerald;       // 翠绿色
	extern Material jade;          // 玉石绿
	extern Material obsidian;      // 黑曜石
	extern Material pearl;         // 珍珠白
	extern Material ruby;          // 红宝石
	extern Material turquoise;     // 绿松石
	extern Material brass;         // 黄铜
	extern Material bronze;        // 青铜
	extern Material chrome;        // 铬
	extern Material copper;        // 铜
	extern Material gold;          // 黄金
	extern Material silver;        // 银
	extern Material black_plastic; // 黑色塑料
	extern Material cyan_plastic;  // 青色塑料
	extern Material green_plastic; // 绿色塑料
	extern Material red_plastic;   // 红色塑料
	extern Material white_plastic; // 白色塑料
	extern Material yellow_plastic;// 黄色塑料
	extern Material black_rubber;  // 黑色橡皮
	extern Material cyan_rubber;   // 青色橡皮
	extern Material green_rubber;  // 绿色橡皮
	extern Material red_rubber;    // 红色橡皮
	extern Material white_rubber;  // 白色橡皮
	extern Material yellow_rubber; // 黄色橡皮
}