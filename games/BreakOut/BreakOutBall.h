#pragma once

#include <glm/glm.hpp>

class BreakOutBall
{
public:
	glm::vec2 position;
	glm::vec2 direction;


	BreakOutBall(glm::vec2 position);

	bool isInGame();

};