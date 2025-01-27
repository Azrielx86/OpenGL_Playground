//
// Created by kaguya on 7/15/24.
//

#ifndef SHADERPLAYGROUND_MESH_H
#define SHADERPLAYGROUND_MESH_H

#include "Material.h"
#include "ResourceManager.h"
#include "Shader.h"
#include <array>
#include <glm/glm.hpp>
#include <vector>

typedef struct vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
} Vertex;

class Mesh
{
  private:
	unsigned int VAO{};
	unsigned int VBO{};
	unsigned int IBO{};

	std::vector<Vertex> vertex;
	std::vector<unsigned int> index;
	std::vector<std::shared_ptr<Texture>> textures;
	Material material{};

  public:
	Mesh(std::vector<Vertex> vertex, std::vector<unsigned int> index, std::vector<std::shared_ptr<Texture>> textures, const Material &material);
	void Load();
	void Render([[maybe_unused]] [[maybe_unused]] const Shader &shader) const;
	[[nodiscard]] Material *GetMaterial();
};

#endif // SHADERPLAYGROUND_MESH_H
