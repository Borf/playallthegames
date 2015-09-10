#pragma once

#include "../../playallthegames/AlivePlayer.h"

namespace asteroids
{

	class Player : public AlivePlayer
	{
	public:
		glm::vec2 position;
		glm::vec2 movement;
		float rotation;


		Player(int index) : AlivePlayer(index)
		{
			movement = glm::vec2(0, 0);
			rotation = 0;
		}
	};


}