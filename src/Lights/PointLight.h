//
// Created by tohka on 7/16/25.
//

#ifndef POINTLIGHT_H
#define POINTLIGHT_H

namespace Lights
{

struct PointLight
{
	glm::vec4 position{};
	glm::vec4 ambient{};
	glm::vec4 diffuse{};
	glm::vec4 specular{};
	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
	GLint isTurnedOn;
};

} // namespace Lights

#endif // POINTLIGHT_H
