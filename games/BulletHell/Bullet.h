#pragma once

#include <glm/glm.hpp>

class Settings;

class Bullet
{
public:
	glm::vec2 pos;
	glm::vec2 dir;
	glm::vec4 color;

	Bullet(glm::vec2 pos, glm::vec2 dir);

	void update(float elapsedTime, Settings* settings);

	glm::mat4 getMatrix();
};

