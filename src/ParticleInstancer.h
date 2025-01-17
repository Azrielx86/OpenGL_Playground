//
// Created by kaguya on 1/11/25.
//

#ifndef PARTICLEINSTANCER_H
#define PARTICLEINSTANCER_H

#include "Entities/Particle.h"
#include "GlobalDefines.h"

class ParticleInstancer
{
  public:
	unsigned int Init();
	unsigned int particleVBO = -1;
	unsigned int centersVBO = -1;
	unsigned int colorsVBO = -1;

	void RenderInstance();

  private:
	static const GLfloat vtx_buffer_data[];
	unsigned int textureId = -1;
	explicit ParticleInstancer(unsigned int textureId);
};

#endif // PARTICLEINSTANCER_H
