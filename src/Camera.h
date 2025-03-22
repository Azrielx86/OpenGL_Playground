//
// Created by kaguya on 6/17/24.
//

#ifndef SHADERPLAYGROUND_CAMERA_H
#define SHADERPLAYGROUND_CAMERA_H

#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include <glm/glm.hpp>

class Camera
{
  private:
	glm::vec3 position;
	glm::vec3 front{};
	glm::vec3 up{};
	glm::vec3 right{};
	glm::vec3 worldUp{};
	float pitch;
	float yaw;
	[[maybe_unused]] float roll{};

	float moveSpeed;
	float turnSpeed;

	Input::Keyboard *keyboard{};
	Input::Mouse *mouse{};

  public:
	Camera(glm::vec3 startPosition, glm::vec3 startUp, float startYaw = -90.0f, float startPitch = 0.0f, float moveSpeed = 0.7f, float turnSpeed = 0.5f);
	void SetInput(Input::Keyboard *kb, Input::Mouse *ms);
	void Move(float deltaTime);
	void Update();
	[[nodiscard]] float GetPitch() const;
	[[nodiscard]] float GetYaw() const;
	glm::vec3 GetPosition();
	glm::vec3 GetDirection();
	glm::mat4 GetLookAt();
	void SetMoveSpeed(float moveSpeed);
	void SetTurnSpeed(float turnSpeed);
};

#endif // SHADERPLAYGROUND_CAMERA_H
