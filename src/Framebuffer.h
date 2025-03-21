//
// Created by kaguya on 3/13/25.
//

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "Resources/Primitive.h"
#include "Shader.h"

#include <memory>

class Framebuffer
{
	unsigned int fbo = 0;
	unsigned int textureId = 0;
	unsigned int rbo = 0;
	std::unique_ptr<Shader> shader;
	Resources::Primitive plane;

  public:
	Framebuffer(Shader &shader, int width, int height);
	~Framebuffer();
	void BeginRender() const;
	static void EnableMainFramebuffer();
	void RenderQuad() const;
	void CreateFramebuffer(int width, int height);
	void DestroyFramebuffer() const;
};

#endif // FRAMEBUFFER_H
