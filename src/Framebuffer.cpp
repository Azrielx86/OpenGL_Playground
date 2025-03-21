//
// Created by kaguya on 3/13/25.
//

#include "Framebuffer.h"

#include "Resources/Primitive.h"
#include "Resources/ResourceManager.h"

Framebuffer::Framebuffer(Shader &shader, const int width, const int height) : plane(Resources::Primitive::planeVertices, Resources::Primitive::planeVerticesSize, Resources::Primitive::planeIndexes, Resources::Primitive::planeIndexesSize)
{
	this->shader = std::make_unique<Shader>(shader);

	CreateFramebuffer(width, height);
}

Framebuffer::~Framebuffer()
{
	DestroyFramebuffer();
}

void Framebuffer::BeginRender() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void Framebuffer::EnableMainFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Framebuffer::RenderQuad() const
{
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, textureId);
	plane.Render(*shader);
}

void Framebuffer::CreateFramebuffer(const int width, const int height)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::DestroyFramebuffer() const
{
	if (fbo != 0)
		glDeleteFramebuffers(1, &fbo);

	if (textureId != 0)
		glDeleteTextures(1, &textureId);

	if (rbo != 0)
		glDeleteRenderbuffers(1, &rbo);
}
