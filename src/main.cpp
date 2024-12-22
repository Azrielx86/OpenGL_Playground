#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
// region Global Include (Must be the first!)
#define STB_IMAGE_IMPLEMENTATION
#include "GlobalDefines.h"
// endregion Global Include
#include "Camera.h"
#include "Input/Keyboard.h"
#include "Mesh.h"
#include "Model.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Window.h"
#include <iostream>
#include <vector>

float deltaTime, lastTime;
float red = 0.0f;
bool enableCursorEvent = true;
bool enableCursor = true;

Input::Keyboard &keyboard = *Input::Keyboard::GetInstance();
Input::Mouse &mouse = *Input::Mouse::GetInstance();
ResourceManager &resources = *ResourceManager::GetInstance();

void ConfigureKeys(Window &window)
{
	keyboard
	    .AddCallback(GLFW_KEY_I, []() -> void
	                 {
		                 red += 0.01f;
		                 if (red > 1) red = 1.0f;
	                 })
	    .AddCallback(GLFW_KEY_K, []() -> void
	                 {
		                 red -= 0.01f;
		                 if (red < 0) red = 0.0f;
	                 })
	    .AddCallback(GLFW_KEY_ESCAPE, [&window]() -> void
	                 {
		                 window.SetShouldClose(true);
	                 })
	    .AddCallback(GLFW_KEY_T, [&window]() -> void
	                 {
		                 if (enableCursorEvent) return;
		                 enableCursorEvent = true;
		                 enableCursor = !enableCursor;
		                 mouse.ToggleMouse(enableCursor);
		                 window.SetMouseStatus(enableCursor);
	                 });
}

int main()
{
	Window window(1280, 720, "OpenGL Playground");

	if (!window.Init())
	{
		std::cerr << "Cannot initialize window.\n";
		return 1;
	}

	resources.ScanResources();
	resources.LoadTextures();

	Model turret("./assets/models/turret/source/turret_model.fbx");

	Shader shader{};
	shader.LoadShader("./shaders/base.vert", "./shaders/base.frag");

	Camera camera({2.0f, 2.0f, 2.0f}, {0.0f, 1.0f, 0.0f});
	camera.SetInput(Input::Keyboard::GetInstance(), Input::Mouse::GetInstance());

	mouse.ToggleMouse(enableCursor);
	window.SetMouseStatus(enableCursor);

	std::vector vertices = {
	    Vertex{.position = {-0.5f, -0.5f, 0.0f}, .normal = {1.0f, 0.0f, 0.0f}, .texCoords = {1.0f, 0.0f}},
	    Vertex{.position = {0.5f, -0.5f, 0.0f}, .normal = {0.0f, 1.0f, 0.0f}, .texCoords = {0.5f, 0.5f}},
	    Vertex{.position = {0.0f, 0.5f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f}, .texCoords = {0.0f, 0.1f}}};

	std::vector<unsigned int> indices = {0, 1, 2};

	Mesh mesh(vertices, indices, std::vector<std::shared_ptr<Texture>>());
	mesh.Load();

	ConfigureKeys(window);

	glm::mat4 view = glm::lookAt(glm::vec3(1.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), window.GetAspect(), 0.1f, 100.0f);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.0f});

	//	auto

	while (!window.ShouldClose())
	{
		auto now = static_cast<float>(glfwGetTime());
		deltaTime = now - lastTime;
		lastTime = now;

		window.StartGui();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Move(deltaTime);

		shader.Use();

		view = camera.GetLookAt();
		projection = glm::perspective(glm::radians(45.0f), window.GetAspect(), 0.1f, 100.0f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, {0.0f, 0.0f, 0.0f});
		model = glm::scale(model, {0.02f, 0.02f, 0.02f});

		shader.Set<3>("inColor", glm::vec3{red, 0.6f, 0.4f});
		shader.Set<4, 4>("model", model);
		shader.Set<4, 4>("view", view);
		shader.Set<4, 4>("projection", projection);

		// shader.Set<3, 2>("test", glm::mat3x2(1.0f));

		// region render

		//		mesh.Render(shader);
		turret.Render(shader);

		// endregion

		// region gui
		ImGui::Begin("Camera info");
		auto camPos = camera.GetPosition();
		auto camDir = camera.GetDirection();
		ImGui::Text("%s", std::format("Position: x={} y={} z={}", camPos.x, camPos.y, camPos.z).c_str());
		ImGui::Text("%s", std::format("Direction: x={} y={} z={}", camDir.x, camDir.y, camDir.z).c_str());
		ImGui::Text("Yaw = %f | Pitch = %f", static_cast<double>(camera.GetYaw()), static_cast<double>(camera.GetPitch()));
		ImGui::End();

		ImGui::Begin("Render data");
		ImGui::Text("Delta time = %f", static_cast<double>(deltaTime));
		ImGui::End();
		// endregion

		keyboard.HandleKeyLoop();

		// region Special keys handle
		if (!keyboard.GetKeyPress(GLFW_KEY_T)) enableCursorEvent = false;
		// endregion

		window.EndGui();
		window.SwapBuffers();
		glfwPollEvents();
	}

	return 0;
}

#pragma clang diagnostic pop