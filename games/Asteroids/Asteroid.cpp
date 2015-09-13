#include "Asteroid.h"
#include <blib/Math.h>

namespace asteroids
{
	Asteroid::Asteroid()
	{
		//initialize at a random spot
		position = glm::vec2(rand() % 1920, rand() % 1080);
		//movement = glm::vec2(
		//	blib::math::randomFloat(-10, 10),			//movement x axis
		//	blib::math::randomFloat(-10, 10));		//movement y axis

		movement = blib::math::randomFloat(7, 10) * blib::math::fromAngle(glm::radians(blib::math::randomFloat(0, 360)));

		angle = rand() % 360;
		angleIncrement = blib::math::randomFloat(-0.05f, 0.05f);  //movement around itself?
		size = rand() % 3;
	}
}