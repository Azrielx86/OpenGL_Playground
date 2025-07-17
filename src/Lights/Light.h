//
// Created by kaguya on 1/25/25.
//

#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

namespace Lights
{

struct Light
{
	glm::vec3 position{};
	glm::vec3 ambient{};
	glm::vec3 diffuse{};
	glm::vec3 specular{};
	float constant;
	float linear;
	float quadratic;
};

} // namespace Lights

#endif // LIGHT_H
