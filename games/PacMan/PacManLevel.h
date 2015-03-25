#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <blib/SpriteBatch.h>


namespace blib { class SpriteSheet; class SpriteBatch; };
namespace pacman
{
	class Enemy;

	class PacManLevel
	{
	public:
		blib::SpriteBatch::Cache* levelCache;
		blib::SpriteBatch::Cache* pelletCache;

		std::vector<std::vector<int> > levelData;
		blib::SpriteSheet* backgroundSheet;

		virtual std::vector<std::vector<int> > getLevel() = 0;
		virtual std::vector<glm::vec2> getPlayerPositions() = 0;
		virtual std::vector<Enemy*> getEnemies() = 0;
		virtual bool inPen(int x, int y) = 0;
		virtual std::vector<glm::vec2> getScorePositions() = 0;



		virtual bool isWalkable(int x, int y, const std::vector<Enemy*> &enemies, float gameTime, Enemy* enemy = NULL);
		PacManLevel();

		void draw(blib::SpriteBatch* spriteBatch);
		bool tryEat(int x, int y);
	};
}