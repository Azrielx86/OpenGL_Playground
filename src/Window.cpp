//
// Created by kaguya on 5/30/24.
//

#include "Window.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include <format>
#include <iostream>

void GLFWwindowDeleter::operator()(GLFWwindow *window) const
{
	glfwDestroyWindow(window);
}

Window::Window(const int width, const int height, const char *name) : height(height), width(width), winName(name)
{
	this->window.reset();
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
#if DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	window.reset(glfwCreateWindow(width, height, winName, nullptr, nullptr));

	if (window == nullptr)
	{
		const char *errorLog = nullptr;
		glfwGetError(&errorLog);
		std::cerr << "Cannot create glfw window: " << errorLog << "\n";
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window.get());
	glfwSetWindowUserPointer(window.get(), this);

	glewExperimental = true;
	if (const auto glewInitResult = glewInit(); glewInitResult != GLEW_OK && glewInitResult != GLEW_ERROR_NO_GLX_DISPLAY)
	{
		std::cerr << "Cannot start GLEW: " << glewGetErrorString(glewInitResult) << "\n";
		return false;
	}

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	monitor = glfwGetPrimaryMonitor();

	// callbacks
	glfwSetFramebufferSizeCallback(window.get(), CbkFrameBufferSize);
	glfwSetCursorPosCallback(window.get(), CbkMouseCallback);
	glfwSetKeyCallback(window.get(), CbkKeyboardInputCallback);

#ifdef DEBUG
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GlDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif

	glfwSetInputMode(window.get(), GLFW_STICKY_KEYS, GLFW_FALSE);
	//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// imgui setting

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
	ImGui_ImplOpenGL3_Init();

	return true;
}

void Window::CbkFrameBufferSize([[maybe_unused]] GLFWwindow *window, int width, int height)
{
	auto pWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
	glfwGetFramebufferSize(window, &pWindow->width, &pWindow->height);

	for (Framebuffer *buffer : pWindow->buffers)
	{
		buffer->CreateFramebuffer(width, height);
	}
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(window.get());
}

void Window::EndRenderPass()
{
	glfwSwapBuffers(window.get());
	glfwPollEvents();
}

Window::~Window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	window.reset();
	glfwTerminate();
}

int Window::GetHeight() const { return height; }

int Window::GetWidth() const { return width; }

void Window::AddFramebuffer(Framebuffer *fb)
{
	buffers.push_back(fb);
}

void Window::ToggleFullscreen()
{
	if (glfwGetWindowMonitor(window.get()) == nullptr)
	{
		glfwGetWindowPos(window.get(), &windowPos[0], &windowPos[1]);
		glfwGetWindowSize(window.get(), &windowSize[0], &windowSize[1]);

		const auto mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window.get(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else
	{
		glfwSetWindowMonitor(window.get(), nullptr, windowPos[0], windowPos[1], windowSize[0], windowSize[1], 0);
	}
}

void Window::CbkMouseCallback([[maybe_unused]] GLFWwindow *window, double xpos, double ypos)
{
	Input::Mouse::GetInstance()->HandleMove(xpos, ypos);
}

void Window::CbkKeyboardInputCallback([[maybe_unused]] GLFWwindow *window, int key, int code, int action, int mode)
{
	Input::Keyboard::GetInstance()->HandleKeys(key, code, action, mode);
}

#ifdef DEBUG
void Window::GlDebugOutput(const GLenum source, const GLenum type, const unsigned int id, const GLenum severity, [[maybe_unused]] GLsizei length, const char *message, [[maybe_unused]] const void *userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "\033[31m";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "\033[33m";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "\033[35m";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cout << "\033[36m";
		break;
	default:;
	}

	std::cout << "===============================================\n";
	std::cout << "Debug message (" << id << "): " << message << "\n";

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		std::cout << "Source: API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		std::cout << "Source: Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		std::cout << "Source: Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		std::cout << "Source: Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		std::cout << "Source: Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		std::cout << "Source: Other";
		break;
	default:;
	}
	std::cout << "\n";

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "Type: Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "Type: Deprecated Behaviour";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "Type: Undefined Behaviour";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "Type: Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "Type: Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		std::cout << "Type: Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		std::cout << "Type: Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		std::cout << "Type: Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "Type: Other";
		break;
	default:;
	}
	std::cout << "\033[0m\n";
}
#endif

void Window::SetShouldClose(bool value) { glfwSetWindowShouldClose(window.get(), value); }

float Window::GetAspect() const { return (float) width / (float) height; }

void Window::SetMouseStatus(bool enable)
{
	if (enable)
		glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
