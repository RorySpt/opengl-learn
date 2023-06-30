#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "shader.h"
class Model
{
public:
	Model(const char* path);
	void Draw(const ShaderProgram& shader) const;
private:
	/* 模型数据 */
	std::vector<Mesh> _meshes;
	std::string _directory;
	mutable  std::vector<Texture> _texturesLoaded;
	/* 函数 */
	void loadModel(std::string_view path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene) const;

	std::vector<Texture> loadMaterialTexture(const aiMaterial* material, aiTextureType type, std::string_view typeName) const;
};

