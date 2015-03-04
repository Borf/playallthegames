#pragma once

#include <glm/glm.hpp>

class Splat
{
public:
	Splat(glm::vec2 pos, float rot);

	glm::vec2 pos;
	float rot;

	glm::mat4 getMatrix();
};