//
// Created by kaguya on 7/20/24.
//

#include "Mouse.h"
#include <format>
#include <iostream>

namespace Input
{

std::unique_ptr<Mouse> Mouse::instance = nullptr;

Mouse *Mouse::GetInstance()
{
	if (!instance) instance.reset(new Mouse());
	return instance.get();
}

void Mouse::HandleMove(double xpos, double ypos)
{
	if (!enable)
	{
		lastx = 0.0;
		lasty = 0.0;
		first = true;
		return;
	}
	
	if (first)
	{
		lastx = xpos;
		lasty = ypos;
		first = false;
	}

	changex = xpos - lastx;
	changey = lasty - ypos;

	lastx = xpos;
	lasty = ypos;
}

double Mouse::GetChangex()
{
	double change = changex;
	changex = 0.0;
	return change;
}

double Mouse::GetChangey()
{
	double change = changey;
	changey = 0.0;
	return change;
}

void Mouse::ToggleMouse(bool inEnable)
{
	enable = inEnable;
}

bool Mouse::IsEnabled() const
{
	return enable;
}
} // namespace Input
