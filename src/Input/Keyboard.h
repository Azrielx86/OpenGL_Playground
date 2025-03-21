//
// Created by kaguya on 7/15/24.
//

#ifndef SHADERPLAYGROUND_KEYBOARD_H
#define SHADERPLAYGROUND_KEYBOARD_H

#include <functional>
#include <memory>
#include <array>
#include <unordered_map>

namespace Input
{

class Keyboard
{
  private:
	typedef struct _key
	{
		bool pressed = false;
		std::function<void()> callback = nullptr;
	} Key;

	std::array<Key, 512> keys;
	static std::unique_ptr<Keyboard> instance;
	Keyboard() = default;

  public:
	Keyboard(Keyboard &) = delete;
	Keyboard operator=(Keyboard &) = delete;
	static Keyboard *GetInstance();
	void HandleKeys(int key, [[maybe_unused]] int code, int action, [[maybe_unused]] int mode);
	Keyboard &AddCallback(int key, std::function<void()> callback);
	void HandleKeyLoop();
	[[nodiscard]] bool GetKeyPress(int key);
};

} // namespace Input

#endif // SHADERPLAYGROUND_KEYBOARD_H
