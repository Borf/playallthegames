#pragma once

#include <glm/glm.hpp>

namespace asteroids
{
	class Asteroid
	{
	public:
		glm::vec2 position;
		glm::vec2 movement;
		float angle;
		float angleIncrement;
		int size;

		float fade;

		Asteroid();
	};

}
