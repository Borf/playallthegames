#pragma once

#include "Enemy.h"

namespace pacman
{
	class FollowGhost : public Enemy
	{
	public:
		FollowGhost(glm::vec2 position, glm::vec2 direction, glm::vec4 color, int penStatus);

		virtual glm::vec2 pickDirection(PacManLevel* level, std::vector<PacManPlayer*> players, std::vector<Enemy*> enemies, float gameTime);

	};
}