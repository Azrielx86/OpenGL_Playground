//
// Created by kaguya on 8/2/24.
//

#include "ResourceManager.h"
#include "../GlobalDefines.h"

#include <array>
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

DefaultResources ResourceManager::defaultResources{};

ResourceManager *ResourceManager::GetInstance()
{
	if (!instance) instance.reset(new ResourceManager());
	return instance.get();
}

void ResourceManager::ScanResources()
{
	ScanTextures();
	ScanShaders();
}

void ResourceManager::LoadTextures()
{
	std::for_each(std::execution::par_unseq,
	              textures.begin(),
	              textures.end(),
	              [](const std::unordered_map<std::string, std::shared_ptr<Texture>, string_hash>::value_type &texture) -> void
	              {
		              if (!texture.second->IsLoaded())
			              texture.second->LoadTexture();
	              });
}

void ResourceManager::InitDefaultResources()
{
	// Default normal
	std::array<uint8_t, 4> data = {255, 255, 255, 255};
	glGenTextures(1, &defaultResources.normal);
	glBindTexture(GL_TEXTURE_2D, defaultResources.normal);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

	// Defatult spacular
	data = {0, 0, 0, 0};
	glGenTextures(1, &defaultResources.specular);
	glBindTexture(GL_TEXTURE_2D, defaultResources.specular);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
	// TODO : Generate default textures
}

void ResourceManager::SetResourcesPath(const char *newPath)
{
	resourcesPath = newPath;
}

const char *ResourceManager::GetResourcesPath() const
{
	return resourcesPath.c_str();
}

void ResourceManager::ScanTextures()
{
	for (const auto &file : std::filesystem::recursive_directory_iterator(resourcesPath))
	{
		if (file.is_directory()) continue;
		std::string path = file.path().string();
		std::cmatch results;
		std::regex_search(path.c_str(), results, std::regex(R"([^/|\\]+\.\w+$)"));

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

void ResourceManager::ScanShaders()
{
	for (const auto &file : std::filesystem::recursive_directory_iterator(shadersPath))
	{
		if (file.is_directory()) continue;

		std::string path = file.path().string();
		std::cmatch results;
		std::regex_search(path.c_str(), results, std::regex(R"([^/|\\]+\.\w+$)"));

		if (results.size() != 1)
		{
			std::cerr << "File is not valid\n";
			continue;
		}

		std::string extPattern = R"(vert|frag|geom|tese|tesc|comp)";
		std::string replacePattern = R"((^.*[/|\\]+)|(\.()" + extPattern + ")$)";
		if (std::regex_search(path, std::regex(extPattern)))
		{
			std::string shaderName = std::regex_replace(path, std::regex(replacePattern), "");
			std::shared_ptr<Shader> shader;
			if (!shaders.contains(shaderName))
			{
				shader = std::make_shared<Shader>();
				shader->CreateProgram();
				shaders[shaderName] = shader;
			}
			else
				shader = shaders[shaderName];

			std::regex_search(path.c_str(), results, std::regex(extPattern + "$"));
			if (results.size() != 1)
			{
				std::cerr << "File is not valid\n";
				continue;
			}
			const auto extension = results[0].str();

			const auto type = ShaderTypeConverter::FromExtension(extension.c_str());

			shader->AttachShader(path.c_str(), type);
		}
	}
}

DefaultResources ResourceManager::GetDefaultResources()
{
	return defaultResources;
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const std::string &filename) const
{
	auto tex = textures.at(filename);
	if (!tex->IsLoaded())
		tex->LoadTexture();
	return tex;
}

std::shared_ptr<Shader> ResourceManager::GetShader(const std::string &shaderName) const
{
	auto shader = shaders.at(shaderName);
	if (!shader->IsLinked())
		shader->LinkProgram();
	return shader;
}
} // namespace Resources
