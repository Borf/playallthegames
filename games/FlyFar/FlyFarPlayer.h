#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"
#include <glm/glm.hpp>

class FlyFarPlayer : public AlivePlayer
{
public:
	FlyFarPlayer(int index);
	glm::vec2 position;
	float rotation;
	float speed;
	float moveUp;

	FlyFarPlayer();


};