#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"
#include <blib/math.h>

class CircleShooterPlayer : public AlivePlayer
{
public:
	glm::vec2 position;

	CircleShooterPlayer(int index) : AlivePlayer(index)
	{
		position = glm::vec2(1920 / 2 + blib::math::randomFloat(-1, 1), 1080 / 2 + blib::math::randomFloat(-1, 1));
	}

};