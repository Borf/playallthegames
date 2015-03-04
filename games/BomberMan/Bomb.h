#pragma once

#include <glm/glm.hpp>

class BomberManPlayer;

class Bomb
{
public:
	glm::vec2 position;
	glm::vec2 movement;
	float timePlaced;
	BomberManPlayer* owner;

	Bomb(glm::vec2 position, float timePlaced, BomberManPlayer* owner)
	{
		this->position = position;
		this->timePlaced = timePlaced;
		this->owner = owner;
		movement = glm::vec2(0,0);
	}
};