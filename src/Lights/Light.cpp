//
// Created by kaguya on 1/25/25.
//

#include "Light.h"

namespace Lights
{
Light::Light()
{
	color = {1.0f, 1.0f, 1.0f};
	position = {0.0f, 0.0f, 0.0f};
}

Light::Light(const glm::vec3 &color, const glm::vec3 &position)
{
	this->color = color;
	this->position = position;
}

void Light::SetPosition(const glm::vec3 &position) { this->position = position; }

void Light::SetColor(const glm::vec3 &color) { this->color = color; }

glm::vec3 Light::GetPosition() const { return position; }

glm::vec3 Light::GetColor() const { return color; }

void Light::SendToShader(Shader &shader, int index) const
{
	// TODO : OPTIMIZE THIS SH*T.
	const std::string uniformName = std::format("directionalLights[{}].", index);
	shader.Set<3>((uniformName + "position").c_str(), position);
	shader.Set<3>((uniformName + "color").c_str(), color);
}
} // namespace Lights