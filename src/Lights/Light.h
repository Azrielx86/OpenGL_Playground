//
// Created by kaguya on 1/25/25.
//

#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>
#include "../Shader.h"

namespace Lights
{

class Light
{
  public:
	Light();
	Light(const glm::vec3 &color, const glm::vec3 &position);

	void SetPosition(const glm::vec3 &position);

	void SetColor(const glm::vec3 &color);

	[[nodiscard]] glm::vec3 GetPosition() const;

	[[nodiscard]] glm::vec3 GetColor() const;

	void SendToShader(Shader& shader, int index) const;

  protected:
	glm::vec3 position{};
	glm::vec3 color{};
};

} // namespace Lights

#endif // LIGHT_H
