//
// Created by kaguya on 1/11/25.
//

#ifndef PARTICLE_H
#define PARTICLE_H
#include <glm/glm.hpp>

namespace Entities
{

struct Particle
{
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec4 color;
	float life;

	Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}

	static unsigned int lastUsedParticle;
	
	// unsigned int FindLastParticle()
	// {
	// 	
	// }
};

} // namespace Entities

#endif // PARTICLE_H
