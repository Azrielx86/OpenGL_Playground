//
// Created by kaguya on 8/2/24.
//

#ifndef SHADERPLAYGROUND_RESOURCEMANAGER_H
#define SHADERPLAYGROUND_RESOURCEMANAGER_H

#include "../Shader.h"
#include "Texture.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace Resources
{

// struct is from "https://www.cppstories.com/2021/heterogeneous-access-cpp20/"
struct string_hash
{
	using is_transparent = void;

	[[nodiscard]] size_t operator()(const char *txt) const
	{
		return std::hash<std::string_view>{}(txt);
	}

	// ReSharper disable once CppParameterMayBeConst
	[[nodiscard]] size_t operator()(std::string_view txt) const
	{
		return std::hash<std::string_view>{}(txt);
	}

	[[nodiscard]] size_t operator()(const std::string &txt) const
	{
		return std::hash<std::string>{}(txt);
	}
};

struct DefaultResources
{
	unsigned int diffuse = 0;
	unsigned int normal = 0;
	unsigned int emission = 0;
	unsigned int specular = 0;
};

class ResourceManager
{
	std::string resourcesPath = "assets";
	std::string shadersPath =
#if DEBUG
	    "../shaders";
#else
	    "shaders";
#endif

	//	std::vector<std::shared_ptr<Texture>> textures;
	std::unordered_map<std::string, std::shared_ptr<Texture>, string_hash> textures;
	std::unordered_map<std::string, std::string, string_hash> models;
	std::unordered_map<std::string, std::shared_ptr<Shader>, string_hash> shaders;
	ResourceManager() = default;
	static std::unique_ptr<ResourceManager> instance;
	// ~ResourceManager();
	static std::mutex mutex;
	static DefaultResources defaultResources;

	void ScanTextures();
	void ScanShaders();

  public:
	static DefaultResources GetDefaultResources();
	std::shared_ptr<Texture> GetTexture(const std::string &filename) const;
	std::shared_ptr<Shader> GetShader(const std::string &shaderName) const;
	ResourceManager(ResourceManager &) = delete;
	ResourceManager operator=(ResourceManager &) = delete;
	static ResourceManager *GetInstance();
	void ScanResources();
	void LoadTextures();
	static void InitDefaultResources();
	void SetResourcesPath(const char *newPath);
	[[nodiscard]] const char *GetResourcesPath() const;

	friend class Texture;
};

} // namespace Resources

#endif // SHADERPLAYGROUND_RESOURCEMANAGER_H
