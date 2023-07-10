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
	unsigned int emissionTex;	// �Է�����ͼ
	unsigned int diffuseTex;	// ��������ͼ
	unsigned int specularTex;	// �������ͼ
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
	extern Material emerald;       // ����ɫ
	extern Material jade;          // ��ʯ��
	extern Material obsidian;      // ����ʯ
	extern Material pearl;         // �����
	extern Material ruby;          // �챦ʯ
	extern Material turquoise;     // ����ʯ
	extern Material brass;         // ��ͭ
	extern Material bronze;        // ��ͭ
	extern Material chrome;        // ��
	extern Material copper;        // ͭ
	extern Material gold;          // �ƽ�
	extern Material silver;        // ��
	extern Material black_plastic; // ��ɫ����
	extern Material cyan_plastic;  // ��ɫ����
	extern Material green_plastic; // ��ɫ����
	extern Material red_plastic;   // ��ɫ����
	extern Material white_plastic; // ��ɫ����
	extern Material yellow_plastic;// ��ɫ����
	extern Material black_rubber;  // ��ɫ��Ƥ
	extern Material cyan_rubber;   // ��ɫ��Ƥ
	extern Material green_rubber;  // ��ɫ��Ƥ
	extern Material red_rubber;    // ��ɫ��Ƥ
	extern Material white_rubber;  // ��ɫ��Ƥ
	extern Material yellow_rubber; // ��ɫ��Ƥ
}