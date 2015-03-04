#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace blib { class SpriteSheet; class SpriteBatch; };

class BattleLevel
{
public:
	blib::SpriteSheet* texture;
	std::vector<std::vector<int> > levelData;
	std::vector<std::pair<glm::vec2, glm::vec2> > enemySpawns;

	virtual std::vector<std::vector<int> > getLevel() = 0;
	virtual std::vector<glm::vec2> getSpawns() = 0;
	virtual std::vector<std::pair<glm::vec2, glm::vec2> > getEnemySpawns() = 0;

	BattleLevel(blib::SpriteSheet* texture);

	void draw(blib::SpriteBatch* spriteBatch);

};
