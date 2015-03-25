#pragma once

#include <glm/glm.hpp>

namespace bugswat
{
	class Splat
	{
	public:
		Splat(glm::vec2 pos, float rot);

		glm::vec2 pos;
		float rot;

		glm::mat4 getMatrix();
	};
}