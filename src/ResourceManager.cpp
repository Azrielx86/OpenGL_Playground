//
// Created by kaguya on 8/2/24.
//

#include "ResourceManager.h"
#include "GlobalDefines.h"
#include <cstring>
#include <filesystem>
#include <format>
#include <iostream>
#include <regex>

std::unique_ptr<ResourceManager> ResourceManager::instance = nullptr;

void Texture::LoadTexture()
{
	int width, height, channels;
	unsigned char *data;

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(fullpath, &width, &height, &channels, 0);

	if (!data)
		throw std::runtime_error(std::format("Canno't load image: {}", fullpath));

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

	stbi_image_free(data);
}

ResourceManager *ResourceManager::GetInstance()
{
	if (!instance) instance.reset(new ResourceManager());
	return instance.get();
}

void ResourceManager::ScanResources()
{
	for (const auto &file : std::filesystem::recursive_directory_iterator(resourcesPath))
	{
		if (file.is_directory()) continue;
		std::string path = file.path().string();
		std::cmatch results;
		std::regex_search(path.c_str(), results, std::regex("[^/]+\\.\\w+$"));

		if (results.size() != 1)
			std::cerr << "Unexpected behavior on file scanning\n";

		auto filename = results[0].str();

		if (std::regex_search(path, std::regex("\\.(jpe?g|png)$")))
		{
			const auto texture = std::make_shared<Texture>();
			texture->fullpath = new char[path.size() + 1],
			texture->filename = new char[filename.size() + 1];
			memcpy(texture->fullpath, path.c_str(), path.size() + 1);
			memcpy(texture->filename, filename.c_str(), filename.size() + 1);
			textures[filename] = texture;
		}
	}
}

void ResourceManager::LoadTextures()
{
	for (const auto &texture : textures)
		texture.second->LoadTexture();
}

// ResourceManager::~ResourceManager()
// {
// 	for (const auto& [name, texture] : textures)
// 	{
// 		delete[] texture->type;
// 	}
// }

std::shared_ptr<Texture> ResourceManager::GetTexture(std::string_view filename)
{
	const auto iter = std::ranges::find_if(textures, [&filename](const auto &s)
	                         {
		                         return filename.compare(s.first);
	                         });
	return iter != textures.end() ? iter->second : throw std::runtime_error(std::format("Cannot find texture: {}", filename));
}
