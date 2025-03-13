#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
// region Global Include (Must be the first!)
#define STB_IMAGE_IMPLEMENTATION
#include "GlobalDefines.h"
// endregion Global Include
#include "Camera.h"
#include "Entities/Particle.h"
#include "Input/Keyboard.h"
#include "Lights/Light.h"
#include "Model.h"
#include "Resources/ResourceManager.h"
#include "Shader.h"
#include "Skybox.h"
#include "Window.h"

#include <iostream>
#include <vector>

#define RGBCOLOR(r, g, b) glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f)

float deltaTime, lastTime;
float red = 0.0f;
bool enableCursorEvent = true;
bool enableCursor = true;

Input::Keyboard &keyboard = *Input::Keyboard::GetInstance();
Input::Mouse &mouse = *Input::Mouse::GetInstance();
Resources::ResourceManager &resources = *Resources::ResourceManager::GetInstance();

GLuint particleVAO;

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

	auto particleTexture = resources.GetTexture("particle.png");

	Model turret("./assets/models/turret/source/turret_model.fbx");
	// Model pod("./assets/models/pod/source/pod.fbx");
	Model twob("./assets/models/2b/2b.fbx");

	// clang-format off
	Skybox skybox({
	    "./assets/textures/skybox/px.png",
	    "./assets/textures/skybox/nx.png",
	    "./assets/textures/skybox/py.png",
	    "./assets/textures/skybox/ny.png",
	    "./assets/textures/skybox/pz.png",
	    "./assets/textures/skybox/nz.png"
	});
	// clang-format on
	skybox.Load();

#ifdef DEBUG
	Shader shader{};
	shader.LoadShader("../shaders/base.vert", "../shaders/base.frag");
	Shader particleShader{};
	particleShader.LoadShader("../shaders/particle_shader.vert", "../shaders/particle_shader.frag");
	Shader skyboxShader{};
	skyboxShader.LoadShader("../shaders/skybox_shader.vert", "../shaders/skybox_shader.frag");
#else
	Shader shader{};
	shader.LoadShader("./shaders/base.vert", "./shaders/base.frag");
	Shader particleShader{};
	particleShader.LoadShader("./shaders/particle_shader.vert", "./shaders/particle_shader.frag");
	Shader skyboxShader{};
	skyboxShader.LoadShader("./shaders/skybox_shader.vert", "./shaders/skybox_shader.frag");
#endif

	Camera camera({2.0f, 2.0f, 2.0f}, {0.0f, 1.0f, 0.0f});
	camera.SetInput(Input::Keyboard::GetInstance(), Input::Mouse::GetInstance());

	Lights::Light exampleLight;
	exampleLight.SetPosition({2.0f, 2.0f, 2.0f});
	exampleLight.SetColor({1.0f, 1.0f, 1.0f});

	mouse.ToggleMouse(enableCursor);
	window.SetMouseStatus(enableCursor);

	constexpr unsigned int particlesCount = 1;
	std::vector<Entities::Particle> particles(particlesCount);

	for (unsigned int i = 0; i < particlesCount; ++i)
		particles.emplace_back();

	ConfigureKeys(window);

	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model;

	while (!window.ShouldClose())
	{
		auto now = static_cast<float>(glfwGetTime());
		deltaTime = now - lastTime;
		lastTime = now;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		window.StartGui();

		camera.Move(deltaTime);

		view = camera.GetLookAt();
		projection = glm::perspective(glm::radians(45.0f), window.GetAspect(), 0.1f, 100.0f);

		skybox
		    .BeginRender(skyboxShader)
		    .SetProjection(projection)
		    .SetView(view)
		    .Render();

		shader.Use();
		shader.Set<4, 4>("view", view);
		shader.Set<4, 4>("projection", projection);
		shader.Set<3>("ambientLightColor", glm::vec3{1.0f, 1.0f, 1.0f});
		// shader.Set<3>("lightColor", RGBCOLOR(147, 112, 219));
		shader.Set<3>("lightColor", exampleLight.GetColor());
		shader.Set<3>("lightPos", exampleLight.GetPosition());

		model = glm::mat4(1.0f);
		model = glm::translate(model, {0.0f, 0.0f, 0.0f});
		// model = glm::rotate(model, static_cast<float>(glfwGetTime()), {0.0f, 1.0f, 0.0f});
		model = glm::scale(model, {0.02f, 0.02f, 0.02f});
		shader.Set<4, 4>("model", model);
		turret.Render(shader);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		model = glm::mat4(1.0f);
		model = glm::translate(model, {0.0f, 1.0f, 1.0f});
		// model = glm::scale(model, {0.1f, 0.1f, 0.1f});
		shader.Set<4, 4>("model", model);
		twob.Render(shader);
		glDisable(GL_BLEND);

		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// particleShader.Use();
		// for (const auto &particle : particles)
		// {
		// 	particleShader.Set<2>("offset", particle.position);
		// 	particleShader.Set<4>("color", particle.color);
		// 	glBindTexture(GL_TEXTURE_2D, particleTexture->id);
		// 	// glBindVertexArray(particleVAO);
		// 	// glDrawArrays(GL_TRIANGLES, 0, 6);
		// 	// glBindVertexArray(0);
		// }

		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// glDisable(GL_BLEND);

		// region gui
		ImGui::Begin("Camera info");
		auto camPos = camera.GetPosition();
		auto camDir = camera.GetDirection();
		ImGui::Text("Position: x=%f y=%f z=%f", static_cast<double>(camPos.x), static_cast<double>(camPos.y), static_cast<double>(camPos.z));
		ImGui::Text("Direction: x=%f y=%f z=%f", static_cast<double>(camDir.x), static_cast<double>(camDir.y), static_cast<double>(camDir.z));
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