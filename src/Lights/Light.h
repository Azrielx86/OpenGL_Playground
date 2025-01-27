//
// Created by kaguya on 1/25/25.
//

#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>

namespace Lights
{

class Light
{
  public:
	void SetPosition(const glm::vec3 &position);

	void SetColor(const glm::vec3 &color);

	[[nodiscard]] glm::vec3 GetPosition() const;

	[[nodiscard]] glm::vec3 GetColor() const;

  private:
	glm::vec3 position{};
	glm::vec3 color{};
};

} // namespace Lights

#endif // LIGHT_H
