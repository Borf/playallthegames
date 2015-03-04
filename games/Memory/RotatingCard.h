#pragma once

#include <glm/glm.hpp>

class RotatingCard
{
public:
	RotatingCard(glm::ivec2 image, glm::vec2 pos, glm::vec2 targetPos);

	glm::ivec2 image;
	glm::vec2 pos;
	glm::vec2 targetPos;
	float rotation;

	void update(float elapsedTime);
};