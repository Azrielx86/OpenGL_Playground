//
// Created by kaguya on 7/20/24.
//

#include "Model.h"
#include <iostream>
#include <regex>

Model::Model(const char *path)
{
	LoadModel(path);
}

void Model::LoadModel(const char *path)
{
	auto importer = Assimp::Importer();
	const auto aiScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
	{
		std::cerr << "No se pudo cargar el modelo: " << importer.GetErrorString() << "\n";
		return;
	}

	LoadNode(aiScene->mRootNode, aiScene);
}

void Model::LoadNode(const aiNode *pNode, const aiScene *pScene) // NOLINT(*-no-recursion)
{
	for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
		LoadMesh(pScene->mMeshes[pNode->mMeshes[i]], pScene);
	for (unsigned int i = 0; i < pNode->mNumChildren; i++)
		LoadNode(pNode->mChildren[i], pScene);
}

void Model::LoadMesh(const aiMesh *mesh, [[maybe_unused]] const aiScene *scene)
{
	std::vector<Vertex> vertices(mesh->mNumVertices);
	std::vector<unsigned int> faces;
	std::vector<std::shared_ptr<Texture>> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		auto meshVertex = mesh->mVertices[i];
		Vertex vertex{
		    .position = {meshVertex.x, meshVertex.y, meshVertex.z},
		    .normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
		    .texCoords = {0.0f, 0.0f}};
		if (mesh->mTextureCoords[0])
			vertex.texCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

		// vertices.insert(vertices.end(), vertex);
		vertices[i] = vertex;
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			faces.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex > 0)
	{
		const auto material = scene->mMaterials[mesh->mMaterialIndex];
		auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	auto nMesh = Mesh(vertices, faces, textures);
	nMesh.Load();
	meshes.push_back(nMesh);
}

void Model::Render(Shader &shader)
{
	for (const Mesh &mesh : meshes)
		mesh.Render(shader);
}

std::vector<std::shared_ptr<Texture>> Model::LoadMaterialTextures(const aiMaterial *material, const aiTextureType type, const std::string& name)
{
	std::vector<std::shared_ptr<Texture>> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString path;
		material->GetTexture(type, i, &path);

		std::cmatch results;
		std::regex_search(path.C_Str(), results, std::regex(R"([^\\]+\.\w+)"));

		if (results.empty())
		{
			std::cerr << std::format("No texture for: {}\n", path.C_Str());
			continue;
		}
		auto texName = results[0].str();
		auto texture = ResourceManager::GetInstance()->GetTexture(texName);
		texture->type = new char[name.length() + 1]{};
		memcpy(texture->type, name.c_str(), name.size() + 1);
		textures.push_back(texture);
	}
	return textures;
}

#pragma clang diagnostic pop
