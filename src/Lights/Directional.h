//
// Created by kaguya on 4/11/25.
//

#ifndef DIRECTIONAL_H
#define DIRECTIONAL_H
#include "../Shader.h"
#include "Light.h"

namespace Lights
{

class Directional : public Light
{
	glm::vec3 direction{};
public:
	void SetDirection(const glm::vec3& direction);
};

} // namespace Lights

#endif // DIRECTIONAL_H
