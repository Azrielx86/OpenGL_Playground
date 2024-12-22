//
// Created by kaguya on 7/20/24.
//

#ifndef SHADERPLAYGROUND_MOUSE_H
#define SHADERPLAYGROUND_MOUSE_H

#include <GLFW/glfw3.h>
#include <memory>

namespace Input
{

class Mouse
{
  private:
	static std::unique_ptr<Mouse> instance;
	Mouse() = default;

	double lastx{};
	double lasty{};
	double changex{};
	double changey{};

	bool first = true;
	bool enable = true;

  public:
	Mouse(const Mouse &) = delete;
	Mouse operator=(const Mouse &) = delete;
	static Mouse *GetInstance();
	void HandleMove(double xpos, double ypos);
	void ToggleMouse(bool inEnable);
	[[nodiscard]] bool IsEnabled() const;
	[[nodiscard]] double GetChangex();
	[[nodiscard]] double GetChangey();
};

} // namespace Input
#endif // SHADERPLAYGROUND_MOUSE_H
