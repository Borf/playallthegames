#include "Splat.h"

#include <glm/gtc/matrix_transform.hpp>


namespace bugswat
{
	Splat::Splat(glm::vec2 pos, float rot)
	{
		this->pos = pos;
		this->rot = rot;
	}

	glm::mat4 Splat::getMatrix()
	{
		glm::mat4 matrix;
		matrix = glm::translate(matrix, glm::vec3(pos, 0));
		matrix = glm::rotate(matrix, glm::degrees(rot), glm::vec3(0, 0, 1));
		return matrix;
	}

}