#pragma once

#include "../../playallthegames/AlivePlayer.h"

namespace shooter
{
	class Player : public AlivePlayer
	{
	public:
		glm::vec2 position;
		glm::vec2 direction;

		float shootTime;
		float health;

		Player(int index) : AlivePlayer(index)
		{
			direction = glm::vec2(1, 0);
			shootTime = 0;
			health = 100;
		}
	};
}