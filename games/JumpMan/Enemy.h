#pragma once

#include <glm/glm.hpp>

namespace jumpman
{
	class Enemy
	{
	public:
		glm::vec2 position;
		virtual void update(float speed) = 0;
	};

}
