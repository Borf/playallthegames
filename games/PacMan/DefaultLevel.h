#pragma once

#include "PacManLevel.h"

namespace pacman
{
	class DefaultLevel : public PacManLevel
	{
	public:

		virtual std::vector<std::vector<int> > getLevel();
		virtual std::vector<glm::vec2> getPlayerPositions();
		virtual bool inPen(int x, int y);
		virtual std::vector<glm::vec2> getScorePositions();

		virtual std::vector<Enemy*> getEnemies();
		virtual bool isWalkable(int x, int y, const std::vector<Enemy*> &enemies, float gameTime, Enemy* enemy = NULL);

	};
}