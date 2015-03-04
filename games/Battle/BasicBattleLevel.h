#pragma once

#include "BattleLevel.h"

class BasicBattleLevel : public BattleLevel
{
public:
	BasicBattleLevel(blib::SpriteSheet* texture);

	std::vector<std::vector<int> > getLevel();
	std::vector<glm::vec2> getSpawns();
	std::vector<std::pair<glm::vec2, glm::vec2> > getEnemySpawns();
};