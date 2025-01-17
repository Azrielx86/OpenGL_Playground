//
// Created by kaguya on 7/15/24.
//

#include "Keyboard.h"

#include <GLFW/glfw3.h>
#include <format>
#include <iostream>
#include <utility>

namespace Input
{

std::unique_ptr<Keyboard> Keyboard::instance = nullptr;

Keyboard *Keyboard::GetInstance()
{
	if (!instance) instance.reset(new Keyboard());
	return instance.get();
}

void Keyboard::HandleKeys(int key, [[maybe_unused]] int code, int action, [[maybe_unused]] int mode)
{
	// if (action == GLFW_RELEASE)
	// 	keys[key].pressed = false;
	// else
	// keys[key].pressed = true;
	keys[key].pressed = action != GLFW_RELEASE;
}

void Keyboard::HandleKeyLoop()
{
	for (const auto &[pressed, callback] : this->keys)
		if (pressed && callback)
			callback();
}

Keyboard &Keyboard::AddCallback(int key, std::function<void()> callback)
{
	keys[key].callback = std::move(callback);
	return *this;
}

bool Keyboard::GetKeyPress(int key) { return keys[key].pressed; }
} // namespace Input