//
// Created by kaguya on 5/30/24.
//

#include "Window.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include <format>
#include <iostream>

Window::Window(const int width, const int height, const char *name) : height(height), width(width), winName(name)
{
	this->window = nullptr;
}

bool Window::Init()
{
	if (!glfwInit())
	{
		const char *errorLog = nullptr;
		glfwGetError(&errorLog);
		std::cerr << "Cannot create glfw window: " << errorLog << "\n";
		glfwTerminate();
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(width, height, winName, nullptr, nullptr);

	if (window == nullptr)
	{
		const char *errorLog = nullptr;
		glfwGetError(&errorLog);
		std::cerr << "Cannot create glfw window: " << errorLog << "\n";
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);

	glewExperimental = true;
	if (const auto glewInitResult = glewInit(); glewInitResult != GLEW_OK && glewInitResult != GLEW_ERROR_NO_GLX_DISPLAY)
	{
		std::cerr << "Cannot start GLEW: " << glewGetErrorString(glewInitResult) << "\n";
		return false;
	}

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	// callbacks
	glfwSetFramebufferSizeCallback(window, CbkFrameBufferSize);
	glfwSetCursorPosCallback(window, CbkMouseCallback);
	glfwSetKeyCallback(window, CbkKeyboardInputCallback);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
	//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// imgui setting

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return true;
}

void Window::CbkFrameBufferSize([[maybe_unused]] GLFWwindow *window, int width, int height)
{
	auto pWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
	glfwGetFramebufferSize(window, &pWindow->width, &pWindow->height);

	for (Framebuffer* buffer : pWindow->buffers)
	{
		buffer->CreateFramebuffer(width, height);
	}
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(window);
}

Window::~Window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

int Window::GetHeight() const { return height; }

int Window::GetWidth() const { return width; }

void Window::AddFramebuffer(Framebuffer *fb)
{
	buffers.push_back(fb);
}

void Window::CbkMouseCallback([[maybe_unused]] GLFWwindow *window, double xpos, double ypos)
{
	Input::Mouse::GetInstance()->HandleMove(xpos, ypos);
}

void Window::CbkKeyboardInputCallback([[maybe_unused]] GLFWwindow *window, int key, int code, int action, int mode)
{
	Input::Keyboard::GetInstance()->HandleKeys(key, code, action, mode);
}

void Window::SetShouldClose(bool value) { glfwSetWindowShouldClose(window, value); }

float Window::GetAspect() const { return (float) width / (float) height; }

void Window::SetMouseStatus(bool enable)
{
	if (enable)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"

void Window::StartGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::EndGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#pragma clang diagnostic pop
