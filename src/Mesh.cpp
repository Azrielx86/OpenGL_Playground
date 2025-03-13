//
// Created by kaguya on 7/15/24.
//

#include "Mesh.h"

#include <GL/glew.h>

#include <utility>

Mesh::Mesh(std::vector<Vertex> vertex, std::vector<unsigned int> index, std::vector<std::shared_ptr<Resources::Texture>> textures, const Resources::Material &material)
    : vertex(std::move(vertex)), index(std::move(index)), textures(std::move(textures)), material(material) {}

Mesh::~Mesh()
{
	if (IBO != 0) glDeleteBuffers(1, &IBO);
	if (VBO != 0) glDeleteBuffers(1, &VBO);
	if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}

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

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, tangent)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, bitangent)));
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#pragma clang diagnostic pop

void Mesh::Render(const Shader &shader) const
{
	glBindTexture(GL_TEXTURE_2D, 0);
	unsigned int texture_idx = 0;
	unsigned int enabled_textures = 0;
	const auto defaultResources = Resources::ResourceManager::GetInstance()->GetDefaultResources();
	for (texture_idx = 0; texture_idx < textures.size(); ++texture_idx)
	{
		const auto texture = textures[texture_idx];

		std::string name{};
		switch (texture->type)
		{
		case aiTextureType_DIFFUSE:
			name = "texture_diffuse";
			enabled_textures |= DIFFUSE;
			break;
		case aiTextureType_SPECULAR:
			name = "texture_specular";
			enabled_textures |= SPECULAR;
			break;
		case aiTextureType_EMISSIVE:
			name = "texture_emissive";
			enabled_textures |= EMISSION;
			break;
		case aiTextureType_NORMALS:
			name = "texture_normal";
			enabled_textures |= NORMAL;
			break;
		default:;
			continue;
		}

		shader.Set(name.c_str(), static_cast<int>(texture_idx));
		glActiveTexture(GL_TEXTURE0 + texture_idx);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}

	if ((enabled_textures & NORMAL) == 0)
	{
		shader.Set("texture_normal", static_cast<int>(texture_idx));
		glActiveTexture(GL_TEXTURE0 + texture_idx);
		glBindTexture(GL_TEXTURE_2D, defaultResources.normal);
		texture_idx++;
	}

	if ((enabled_textures & SPECULAR) == 0)
	{
		shader.Set("texture_specular", static_cast<int>(texture_idx));
		glActiveTexture(GL_TEXTURE0 + texture_idx);
		glBindTexture(GL_TEXTURE_2D, defaultResources.specular);
		texture_idx++;
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(index.size()), GL_UNSIGNED_INT, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	for (unsigned int i = 0; i < texture_idx; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
}

Resources::Material *Mesh::GetMaterial()
{
	return &this->material;
}
