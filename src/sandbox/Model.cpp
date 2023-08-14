#include "stdafx.h"
#include <ranges>
#include "common.h"
#include "Model.h"

Model::Model(const char* path)
{
	loadModel(path);
}

void Model::Draw(const ShaderProgram& shader) const
{
	for (auto& mesh : _meshes)
	{
		mesh.Draw(shader);
	}
}

void Model::loadModel(std::string_view path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if(!scene||scene->mFlags&AI_SCENE_FLAGS_INCOMPLETE||!scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	std::string_view::size_type index;
	if (!((index = path.find_last_of('/')) == std::string_view::npos
		&&(index = path.find_last_of('\\')) == std::string_view::npos))
	{
		std::cout << "The file path may be incorrectness, path: " << path << '\n';
	}
	_directory = path.substr(0, index);
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// 处理节点所有的网格（如果有的话）
	for(unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(processMesh(mesh, scene));
	}
	// 对它的子节点递归
	for(unsigned int i = 0; i < node->mNumChildren;++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) const
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// 处理顶点位置、法线和纹理坐标
	for(unsigned int i = 0;i<mesh->mNumVertices;++i)
	{
		const aiVector3D* positions = mesh->mVertices;
		const aiVector3D* normals = mesh->mNormals;
		const aiVector3D* texCoords = mesh->mTextureCoords[0];
		vertices.emplace_back(
			glm::vec3{ positions[i].x, positions[i].y, positions[i].z },
			glm::vec3{ normals[i].x, normals[i].y, normals[i].z },
			texCoords ? glm::vec2{ texCoords[i].x, texCoords[i].y } : glm::vec2{ 0,0 }
		);
	}

	// 处理索引
	for(unsigned int i = 0;i<mesh->mNumFaces;++i)
	{
		const aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0;j<face.mNumIndices;++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	//for(const aiFace& face: std::ranges::views::counted(mesh->mFaces, mesh->mNumFaces))
	//{
	//	for(const unsigned int index: std::ranges::views::counted(face.mIndices, face.mNumIndices))
	//	{
	//		indices.push_back(index);
	//	}
	//}
	// 处理材质
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTexture(material,aiTextureType_DIFFUSE,"diffuseTex");
		textures.insert(std::end(textures), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMap = loadMaterialTexture(material, aiTextureType_SPECULAR, "specularTex");
		textures.insert(std::end(textures), specularMap.begin(), specularMap.end());

	}
	return {std::move(vertices),std::move(indices),std::move(textures)};
}

std::vector<Texture> Model::loadMaterialTexture(const aiMaterial* material, aiTextureType type, std::string_view typeName) const
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		bool skip = false;
		for(const auto& loaded:_texturesLoaded)
		{
			if(loaded.type == std::string_view(str.C_Str(),str.length))
			{
				skip = true;
			}
		}
		if(!skip)
		{
			Texture texture;
			texture.id = comm::loadTexture({ str.C_Str(), str.length }, _directory,false);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			_texturesLoaded.push_back(std::move(texture));
		}
	}
	return textures;
}
