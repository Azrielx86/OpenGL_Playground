//
// Created by kaguya on 1/27/25.
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include "assimp/material.h"

namespace Resources {

class Texture
{
	bool loaded = false;
public:
	char *fullpath{};
	char *filename{};
	unsigned int id{};
	// char *type;
	aiTextureType type{};

	void LoadTexture();
	[[nodiscard]] bool IsLoaded() const;

	~Texture();
};
} // Resources

#endif //TEXTURE_H
