//
// Created by kaguya on 1/11/25.
//

#include "ParticleInstancer.h"

// clang-format off
const GLfloat ParticleInstancer::vtx_buffer_data[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
};
// clang-format on

ParticleInstancer::ParticleInstancer(const unsigned int textureId) : textureId(textureId)
{
	
}

unsigned int ParticleInstancer::Init()
{
	glGenBuffers(1, &particleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
	
	return 0;
}

void ParticleInstancer::RenderInstance()
{
	
}
