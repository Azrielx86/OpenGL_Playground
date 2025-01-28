//
// Created by kaguya on 1/20/25.
//

#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/vec3.hpp>

namespace Resources
{

struct Material
{
	glm::vec3 baseColor;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 emissive;
	float shininess;
	bool textured;
};

} // namespace Resources
#endif // MATERIAL_H
