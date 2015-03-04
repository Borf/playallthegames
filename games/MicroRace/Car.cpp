#include "Car.h"


Car::Car( glm::vec2 position, blib::Texture* sprite, glm::vec4 color )
{
	this->position = position;
	this->sprite = sprite;
	this->color = color;
}

void Car::update( float elapsedTime, float speed )
{
	position = glm::vec2(position.x - 3 * (1 + (speed - 1) * 0.9f) * elapsedTime*60, position.y);
}

