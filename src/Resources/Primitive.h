//
// Created by kaguya on 3/20/25.
//

#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include "../Shader.h"

namespace Resources
{
class Primitive
{
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ibo = 0;
	unsigned int count = 0;

  public:
	static float planeVertices[];
	static unsigned int planeVerticesSize;
	static unsigned int planeIndexes[];
	static unsigned int planeIndexesSize;


	Primitive(const float * vertices, unsigned int vertexCount, const unsigned int* indexes, unsigned int indexCount);
	void Render(const Shader& shader) const;
	~Primitive();
};

} // namespace Resources

#endif // PRIMITIVE_H
