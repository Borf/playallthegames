#include "BouncyObject.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 BouncyObject::getMatrix()
{
	return glm::translate(glm::mat4(), glm::vec3(pos,0));
}

