//
// Created by kaguya on 5/30/24.
//

#ifndef SHADERPLAYGROUND_WINDOW_H
#define SHADERPLAYGROUND_WINDOW_H

#include "Framebuffer.h"

#include "Framebuffer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>

struct GLFWwindowDeleter
{
	void operator()(GLFWwindow *window) const;
};

class Window
{
  public:
	Window(int width, int height, const char *name);
	bool Init();
	bool ShouldClose();
	void SetShouldClose(bool value);
	void SwapBuffers();
	[[nodiscard]] float GetAspect() const;
	void SetMouseStatus(bool enable);
	void StartGui();
	void EndGui();
	~Window();
	[[nodiscard]] int GetHeight() const;
	[[nodiscard]] int GetWidth() const;
	void AddFramebuffer(Framebuffer* fb);

  private:
	int height;
	int width;
	const char *winName;
	std::unique_ptr<GLFWwindow, GLFWwindowDeleter> window;
	std::vector<Framebuffer*> buffers;
	static void CbkFrameBufferSize([[maybe_unused]] GLFWwindow *window, int width, int height);
	static void CbkMouseCallback([[maybe_unused]] [[maybe_unused]] GLFWwindow *window, double xpos, double ypos);
	static void CbkKeyboardInputCallback([[maybe_unused]] GLFWwindow *window, int key, [[maybe_unused]] int code, int action, [[maybe_unused]] int mode);
};

#endif // SHADERPLAYGROUND_WINDOW_H
