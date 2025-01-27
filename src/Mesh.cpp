//
// Created by kaguya on 7/15/24.
//

#include "Mesh.h"

#include <GL/glew.h>

#include <utility>

Mesh::Mesh(std::vector<Vertex> vertex, std::vector<unsigned int> index, std::vector<std::shared_ptr<Texture>> textures, const Material &material)
    : vertex(std::move(vertex)), index(std::move(index)), textures(std::move(textures)), material(material) {}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"

void Mesh::Load()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, static_cast<int>(vertex.size() * sizeof(Vertex)), vertex.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<int>(index.size() * sizeof(int)), index.data(), GL_STATIC_DRAW);

	// ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void *>(0)); // NOLINT(*-use-nullptr)
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, texCoords)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#pragma clang diagnostic pop

void Mesh::Render(const Shader &shader) const
{
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		const auto texture = textures[i];

		std::string name{};
		switch (texture->type)
		{
		case aiTextureType_DIFFUSE:
			name = "texture_diffuse";
			break;
		case aiTextureType_SPECULAR:
			name = "texture_specular";
			break;
		case aiTextureType_EMISSIVE:
			name = "texture_emissive";
			break;
		default:;
		}

		shader.Set(name.c_str(), static_cast<int>(i));
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}
	// glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(index.size()), GL_UNSIGNED_INT, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Material *Mesh::GetMaterial()
{
	return &this->material;
}
