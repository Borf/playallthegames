#pragma once

#include <glm/glm.hpp>

class BouncyObject
{
public:
	glm::vec2 pos;
	float speed;
	int sprite;

	glm::mat4 getMatrix();
};