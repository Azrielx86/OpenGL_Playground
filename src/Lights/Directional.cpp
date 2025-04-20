//
// Created by kaguya on 4/11/25.
//

#include "Directional.h"

namespace Lights {
void Directional::SetDirection(const glm::vec3 &direction)
{
	this->direction = direction;
}

} // Lights