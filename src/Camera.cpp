//
// Created by kaguya on 6/17/24.
//

#include "Camera.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch, float moveSpeed, float turnSpeed)
    : position(startPosition), worldUp(startUp), pitch(startPitch), yaw(startYaw), moveSpeed(moveSpeed), turnSpeed(turnSpeed)
{
	front = {0.0f, 0.0f, -1.0f};
}

void Camera::SetInput(Input::Keyboard *kb, Input::Mouse *ms)
{
	keyboard = kb;
	mouse = ms;
}

void Camera::Move(float deltaTime)
{
	const auto velocity = moveSpeed * deltaTime;
	if (keyboard->GetKeyPress(GLFW_KEY_W))
		position += front * velocity * deltaTime;
	if (keyboard->GetKeyPress(GLFW_KEY_S))
		position -= front * velocity * deltaTime;
	if (keyboard->GetKeyPress(GLFW_KEY_A))
		position -= right * velocity * deltaTime;
	if (keyboard->GetKeyPress(GLFW_KEY_D))
		position += right * velocity * deltaTime;
	if (keyboard->GetKeyPress(GLFW_KEY_E))
		position.y += 1.0f * velocity * deltaTime;
	if (keyboard->GetKeyPress(GLFW_KEY_Q))
		position.y -= 1.0f * velocity * deltaTime;

	yaw += (float) mouse->GetChangex() * turnSpeed * deltaTime;
	pitch += (float) mouse->GetChangey() * turnSpeed * deltaTime;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	
	Update();
}

glm::vec3 Camera::GetPosition() { return position; }

glm::vec3 Camera::GetDirection() { return front; }

void Camera::Update()
{
	front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	front.y = glm::sin(glm::radians(pitch));
	front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::GetLookAt()
{
	return glm::lookAt(position, position + front, up);
}

void Camera::SetMoveSpeed(const float moveSpeed) { this->moveSpeed = moveSpeed; }

void Camera::SetTurnSpeed(const float turnSpeed) { this->turnSpeed = turnSpeed; }

float Camera::GetPitch() const
{
	return pitch;
}

float Camera::GetYaw() const
{
	return yaw;
}
