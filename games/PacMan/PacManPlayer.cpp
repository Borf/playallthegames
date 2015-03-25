#include "PacManPlayer.h"

namespace pacman
{
	PacManPlayer::PacManPlayer(int index) : AlivePlayer(index)
	{
		direction = glm::vec2(0, 0);
		newDirection = glm::vec2(0, 0);
		score = 0;
		offset = 0;
	}


}