//
// Created by kaguya on 7/15/24.
//

#ifndef SHADERPLAYGROUND_MESH_H
#define SHADERPLAYGROUND_MESH_H

#include "Resources/Material.h"
#include "Resources/ResourceManager.h"
#include "Resources/Texture.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>

typedef struct vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;
} Vertex;

class Mesh
{
  public:
	enum HAS_TEXTURE
	{
		DIFFUSE = 0b0001,
		NORMAL = 0b0010,
		SPECULAR = 0b0100,
		EMISSION = 0b1000
	};

  private:
	unsigned int VAO{};
	unsigned int VBO{};
	unsigned int IBO{};

	std::vector<Vertex> vertex;
	std::vector<unsigned int> index;
	std::vector<std::shared_ptr<Resources::Texture>> textures;
	Resources::Material material{};

  public:
	Mesh(std::vector<Vertex> vertex, std::vector<unsigned int> index, std::vector<std::shared_ptr<Resources::Texture>> textures, const Resources::Material &material);
	~Mesh();
	void Load();
	void Render([[maybe_unused]] [[maybe_unused]] const Shader &shader) const;
	[[nodiscard]] Resources::Material *GetMaterial();
};

#endif // SHADERPLAYGROUND_MESH_H
