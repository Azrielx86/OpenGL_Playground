//
// Created by kaguya on 7/20/24.
//

#ifndef SHADERPLAYGROUND_MODEL_H
#define SHADERPLAYGROUND_MODEL_H

#include "Resources/Material.h"

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>

class Model
{
  public:
	explicit Model(const char* path);
	void Render(Shader & shader);
	
  private:
	std::vector<Mesh> meshes;
	void LoadModel(const char* path);
	void LoadNode(const aiNode *pNode, const aiScene *pScene);
	void LoadMesh(const aiMesh *mesh, [[maybe_unused]] [[maybe_unused]] [[maybe_unused]] const aiScene *scene);
	static std::vector<std::shared_ptr<Resources::Texture>> LoadMaterialTextures(const aiMaterial *material, aiTextureType type);
	static Resources::Material LoadMaterial(const aiMaterial* assimpMaterial);
};

#endif // SHADERPLAYGROUND_MODEL_H
