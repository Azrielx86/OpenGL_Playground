//
// Created by kaguya on 8/2/24.
//

#include "ResourceManager.h"
#include "../GlobalDefines.h"
#include <cstring>
#include <execution>
#include <filesystem>
#include <format>
#include <iostream>
#include <regex>

namespace Resources
{

std::unique_ptr<ResourceManager> ResourceManager::instance = nullptr;

std::mutex ResourceManager::mutex;

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
	// for (const auto &texture : textures)
	// 	texture.second->LoadTexture();

	std::for_each(std::execution::par_unseq,
	              textures.begin(),
	              textures.end(),
	              [](const std::unordered_map<std::string, std::shared_ptr<Texture>, string_hash>::value_type &texture) -> void
	              {
		              if (!texture.second->IsLoaded())
			              texture.second->LoadTexture();
	              });
}

void ResourceManager::SetResourcesPath(const char *newPath)
{
	resourcesPath = newPath;
}

const char *ResourceManager::GetResourcesPath() const
{
	return resourcesPath.c_str();
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const std::string &filename) const
{
	auto tex = textures.at(filename);
	if (!tex->IsLoaded())
		tex->LoadTexture();
	return tex;
}
} // namespace Resources
