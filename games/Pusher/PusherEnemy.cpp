#include "PusherEnemy.h"
#include "blib/math.h"
PusherEnemy::PusherEnemy(glm::vec2 position, bool isComingFromLeft, float *speed)
{
    this->isComingFromLeft = isComingFromLeft;
    this->position = position;
	this->speed = speed;
}

void PusherEnemy::update(float elapsedTime)
{
    if (isComingFromLeft) 
		position.x += *speed * elapsedTime;
    else 
		position.x -= *speed * elapsedTime;
}

blib::math::Rectangle PusherEnemy::hitbox()
{
	if (isComingFromLeft)
		return blib::math::Rectangle(glm::vec2(position.x - 115, position.y + 50), 150, 75);
	else
		return blib::math::Rectangle(glm::vec2(position.x + 20, position.y + 50), 150, 75);
}