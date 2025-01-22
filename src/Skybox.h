//
// Created by kaguya on 1/21/25.
//

#ifndef SKYBOX_H
#define SKYBOX_H
#include "GlobalDefines.h"
#include "Shader.h"
#include <array>

class Skybox
{
	std::array<const char *, 6> textures = {};
	unsigned int textureId = -1;
	unsigned int VAO = -1;
	unsigned int VBO = -1;
	static float skyboxVertices[];
	Shader* shader{};

  public:
	explicit Skybox(const std::array<const char *, 6> &textures);
	void Load();
	Skybox &BeginRender(Shader &shader);
	Skybox &SetProjection(const glm::mat4& matrix, const char *name = "projection");
	Skybox &SetView(const glm::mat4 &matrix, const char *name = "view");
	void Render() const;
	~Skybox();
};

#endif // SKYBOX_H
