#pragma once

#include <glm/glm.hpp>

class Sphere
{
public:

	glm::vec2 position;
	glm::vec2 oldPosition;
	float size;

	float getMass();
	void setMass(float mass);
	glm::mat4 getMatrix();

	bool operator ==(const Sphere& other);
};