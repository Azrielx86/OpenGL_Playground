//
// Created by kaguya on 3/20/25.
//

#include "Primitive.h"

#include "../../cmake-build-debug-nosanitize/_deps/imgui_external-src/imstb_truetype.h"

#include <GL/glew.h>

namespace Resources
{
// clang-format off
float Primitive::planeVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f,  -1.0f,  0.0f, 0.0f,
	 1.0f,  -1.0f,  1.0f, 0.0f,

	// -1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f,  1.0f,  1.0f, 1.0f,
	 // 1.0f,  -1.0f,  1.0f, 0.0f
};

unsigned int Primitive::planeVerticesSize = 24;

unsigned int Primitive::planeIndexes[] = {
	0, 1, 2,
	0, 3, 2
};

unsigned int Primitive::planeIndexesSize = 6;
// clang-format on

Primitive::Primitive(const float *vertices, const unsigned int vertexCount, const unsigned int *indexes, const unsigned int indexCount)
{
	count = indexCount;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexCount * sizeof(float)), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<int>(indexCount * sizeof(int)), indexes, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void *>(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(sizeof(float) * 2));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Primitive::Render(const Shader &shader) const
{
	shader.Use();
	// glBindVertexArray(vao);
	// glDisable(GL_DEPTH_TEST);
	// // TODO : Fix count using indices
	// glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Primitive::~Primitive()
{
	if (ibo != 0) glDeleteBuffers(1, &ibo);
	if (vbo != 0) glDeleteBuffers(1, &vbo);
	if (vao != 0) glDeleteVertexArrays(1, &vao);
}

} // namespace Resources