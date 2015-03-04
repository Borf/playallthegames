#pragma once

#include <glm/glm.hpp>
#include <blib/math/Rectangle.h>

class Settings
{
public:
	Settings();
	void save();
	void setSizes();


	int resX;
	int resY;
	bool fullscreen;
	bool vsync;
	bool showInstructions;

	float scale;
	glm::vec2 size;
	glm::vec2 center;

	glm::mat4 scaleMatrix;

	blib::math::Rectangle screenRect;
};
