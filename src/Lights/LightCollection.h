//
// Created by kaguya on 4/11/25.
//

#ifndef LIGHTCOLLECTION_H
#define LIGHTCOLLECTION_H

#include "Directional.h"
#include "Light.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace Lights
{

template <typename T>
class LightCollection
{
	static_assert(std::is_base_of_v<Light, T>, "T must be Light-like type");
	using LightPair = std::pair<std::shared_ptr<T>, bool>;

	std::vector<LightPair> lightsVector;
	std::vector<std::shared_ptr<T>> activeLights;
	T *lightArray;
	unsigned int currentCount;
	unsigned int maxCount;
	unsigned int bindingPointIndex;
	std::string prefix;
	Shader* shader{};

	// Stuff for the SSBO
	GLuint ssbo = 0;

  public:
	explicit LightCollection(Shader &shader, const unsigned int bindingPointIndex, const unsigned int maxCount = 4)
	{
		currentCount = 0;
		this->maxCount = maxCount;
		this->bindingPointIndex = bindingPointIndex;
		this->shader = &shader;
		lightArray = nullptr;

		if constexpr (std::is_same_v<Light, T>)
			prefix = "lights";
		else if (std::is_same_v<Directional, T>)
			prefix = "directionalLights";
		else
			prefix = "NA"; // This or throw an exception...?

		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * maxCount, lightArray, GL_DYNAMIC_COPY);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// glCreateBuffers(1, &ssbo);
		// glNamedBufferStorage(ssbo, sizeof(T) * activeLights.size(), static_cast<const void *>(activeLights.data()), GL_DYNAMIC_STORAGE_BIT);
	}

	LightCollection &Add(const T &light)
	{
		lightsVector.push_back(std::make_pair(std::make_shared<T>(light), true));
		currentCount++;
		return *this;
	}

	[[nodiscard]] unsigned int GetCurrentCount() const { return currentCount; }

	// void Remove(const T &light) {  }

	T *operator[](int index)
	{
		auto &light = lightsVector.at(index);
		return light.first.get();
	}

	void ToggleLight(const int index, const bool state)
	{
		const bool isTurnedOn = lightsVector.at(index).second;
		if (state == isTurnedOn) return;
		lightsVector.at(index).second = !isTurnedOn;
		UpdateActiveLights();
	}

	void UpdateActiveLights()
	{
		lightArray = new T[lightsVector.size()];
		unsigned int i = 0;

		for (const auto &[light, isActive] : lightsVector)
		{
			if (isActive)
			{
				lightArray[i] = *light.get();
				i++;
			}
		}

		// glNamedBufferSubData(ssbo, 0, sizeof(T) * activeLights.size(), static_cast<void *>(activeLights.data()));

		const auto blockIndex = shader->GetProgramResourceIndex(prefix.c_str());
		shader->StorageBlockBinding(blockIndex, bindingPointIndex);
		shader->Set((prefix + "Count").c_str(), static_cast<int>(i));

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		memcpy(p, lightArray, i * sizeof(T));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	// void SendToShader(const Shader &shader)
	// {
	// 	// shader.Set(prefix.append("Count").c_str(), static_cast<int>(currentCount));
	// }
};

} // namespace Lights

#endif // LIGHTCOLLECTION_H
