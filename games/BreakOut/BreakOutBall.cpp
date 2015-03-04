#include "BreakOutBall.h"


bool BreakOutBall::isInGame()
{
	return position.y < 1000;
}

BreakOutBall::BreakOutBall( glm::vec2 position )
{
	this->position = position;
	this->direction = glm::vec2(0.1f, 1);
}

