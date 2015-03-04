#pragma once

#include "Car.h"

class Foot : public Car
{
public:
	Foot(glm::vec2 position, blib::Texture* sprite, glm::vec4 color);
	virtual void update( float elapsedTime, float speed );

};