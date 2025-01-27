//
// Created by kaguya on 1/25/25.
//

#include "Light.h"

namespace Lights
{
void Light::SetPosition(const glm::vec3 &position) { this->position = position; }

void Light::SetColor(const glm::vec3 &color) { this->color = color; }

glm::vec3 Light::GetPosition() const { return position; }

glm::vec3 Light::GetColor() const { return color; }
} // namespace Lights