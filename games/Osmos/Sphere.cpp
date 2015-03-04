#include "Sphere.h"

#include <glm/gtc/matrix_transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <blib/util/Profiler.h>

float Sphere::getMass()
{
	return (float)M_PI * (size/2) * (size/2);
}

void Sphere::setMass( float mass )
{
	size = (float)sqrt(mass / M_PI) * 2;
}

glm::mat4 Sphere::getMatrix()
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, glm::vec3(position,0));
	matrix = glm::rotate(matrix, (float)glm::degrees(sin(blib::util::Profiler::getAppTime())), glm::vec3(0,0,1));
	matrix = glm::scale(matrix, glm::vec3(size/500, size/500, size/500));
	return matrix;
}

bool Sphere::operator==( const Sphere& other )
{
	return position == other.position && size == other.size;
}

