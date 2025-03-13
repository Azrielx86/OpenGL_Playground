//
// Created by kaguya on 1/27/25.
//

#include "Texture.h"

#include "../GlobalDefines.h"
#include "ResourceManager.h"

#include <format>
#include <stdexcept>

namespace Resources
{

bool Texture::IsLoaded() const { return loaded; }

void Texture::LoadTexture()
{
	int width, height, channels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(fullpath, &width, &height, &channels, 0);

	if (!data)
		throw std::runtime_error(std::format("Cant load image: {}", fullpath));

	ResourceManager::mutex.lock();

	glGenTextures(1, &id);
	if (id == 0)
		throw std::runtime_error("Canno't generate texture!");
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, channels == 3 ? GL_RGB : GL_RGBA, width, height, 0, channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	ResourceManager::mutex.unlock();
	stbi_image_free(data);

	loaded = true;
}

Texture::~Texture()
{
	delete[] fullpath;
	delete[] filename;
}
} // namespace Resources