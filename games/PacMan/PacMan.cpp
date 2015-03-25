#include "PacMan.h"

#include <blib/SpriteSheet.h>
#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/ResourceManager.h>
#include <blib/linq.h>
#include <blib/Color.h>
#include <blib/Renderer.h>
#include <blib/Math.h>
#include <blib/util/Log.h>
#include <blib/Util.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#include "PacManLevel.h"
#include "DefaultLevel.h"
#include "Enemy.h"

using blib::util::Log;

namespace pacman
{
	PacMan::PacMan()
	{
		backgroundColor = glm::vec4(0, 0, 0, 1);
	}

	std::string PacMan::getName()
	{
		return "PacMan";
	}

	std::string PacMan::getInstructions()
	{
		return "Eat the little pills";
	}


	void PacMan::loadResources()
	{
		backgroundSheet = resourceManager->getResource<blib::SpriteSheet>("assets/games/PacMan/tiles");
		pacmanSprite = resourceManager->getResource<blib::SpriteSheet>("assets/games/PacMan/pacman");
		ghostSprite = resourceManager->getResource<blib::Texture>("assets/games/PacMan/ghost.png");
		ghostSpriteUnder = resourceManager->getResource<blib::Texture>("assets/games/PacMan/ghostUnder.png");
		font = resourceManager->getResource<blib::Font>("lindsey");
	}

	void PacMan::start(Difficulty difficulty)
	{
		level = new DefaultLevel();
		level->levelData = level->getLevel();
		level->backgroundSheet = backgroundSheet;

		enemies = level->getEnemies();
		std::vector<glm::vec2> positions = level->getPlayerPositions();
		std::vector<glm::vec2> scorePositions = level->getScorePositions();

		for (auto p : players)
		{
			p->position = glm::round(positions[p->index] / 32.0f);
			p->scorePosition = scorePositions[p->index];
		}
		playTime = 0;
	}

	void PacMan::update(float elapsedTime)
	{
		//elapsedTime *= 25;

		playTime += elapsedTime;
		for (auto p : players)
		{
			if (!p->alive)
				continue;
			if (p->joystick.leftStick.x < -0.5f)
				p->newDirection = glm::vec2(-1, 0);
			if (p->joystick.leftStick.x > 0.5f)
				p->newDirection = glm::vec2(1, 0);
			if (p->joystick.leftStick.y < -0.5f)
				p->newDirection = glm::vec2(0, -1);
			if (p->joystick.leftStick.y > 0.5f)
				p->newDirection = glm::vec2(0, 1);


			float distanceLeft = elapsedTime * 6; // speed!
			while (distanceLeft > 0)
			{
				if ((p->offset == 0 && p->newDirection != glm::vec2(0, 0)))
				{
					if (level->isWalkable((int)(p->position.x + p->newDirection.x), (int)(p->position.y + p->newDirection.y), enemies, playTime))
						p->direction = p->newDirection;
				}
				if (p->newDirection == -p->direction && p->offset > 0)
				{
					p->position = p->position + p->direction;
					p->offset = 1 - p->offset;
					p->direction = p->newDirection;
				}

				if (p->direction != glm::vec2(0, 0))
				{
					p->offset += distanceLeft;
					if (p->offset > 1)
					{
						distanceLeft = p->offset - 1;
						p->position += p->direction;
						p->offset = 0;

						if (!level->isWalkable((int)(p->position.x + p->direction.x), (int)(p->position.y + p->direction.y), enemies, playTime))
							if (level->isWalkable((int)(p->position.x + p->newDirection.x), (int)(p->position.y + p->newDirection.y), enemies, playTime))
								p->direction = p->newDirection;
							else
								p->direction = glm::vec2(0, 0);
					}
					else
						distanceLeft = 0;
				}
				else
					distanceLeft = 0;
			}


			//eat stuff
			if (level->tryEat((int)p->position.x, (int)p->position.y))
				p->score++;

			for (auto e : enemies)
			{
				if (glm::length(e->position*32.0f + 32.0f*e->direction*e->offset - p->position*32.0f + 32.0f*p->direction*p->offset) <= 5)
					p->alive = false;
			}

		}
		for (auto e : enemies)
			e->update(level, players, enemies, playTime, elapsedTime);

	}

	void PacMan::draw()
	{
		spriteBatch->begin(settings->scaleMatrix);
		level->draw(spriteBatch);

		for (auto p : players)
		{
			spriteBatch->draw(font, blib::util::toString(p->score), blib::math::easyMatrix(p->scorePosition), p->participant->color);

			if (!p->alive)
				continue;
			int dir = 0;
			if (glm::length(p->direction) > 0.1f)
				dir = (p->direction.x == 0 ? 2 : 0) + (((p->direction.x - p->direction.y) / 4 > 0) ? 0 : 1);
			else
				dir = (p->newDirection.x == 0 ? 2 : 0) + (((p->newDirection.x - p->newDirection.y) / 4 > 0) ? 0 : 1);
			spriteBatch->draw(pacmanSprite->getFrame(glm::length(p->direction) > 0.1 ? (int)(playTime * 24) % pacmanSprite->spriteCountX : 0, dir), blib::math::easyMatrix(p->position*32.0f + p->direction*p->offset*32.0f + glm::vec2(16, 16)), pacmanSprite->spriteCenter, p->participant->color);
		}

		for (auto e : enemies)
		{
			spriteBatch->draw(ghostSpriteUnder, blib::math::easyMatrix(e->position*32.0f + e->direction*e->offset*32.0f + glm::vec2(16, 16)), ghostSpriteUnder->center);
			spriteBatch->draw(ghostSprite, blib::math::easyMatrix(e->position*32.0f + e->direction*e->offset*32.0f + glm::vec2(16, 16)), ghostSpriteUnder->center, blib::math::Rectangle(0, 0, 1, 1), e->color);
			//spriteBatch->draw(font, blib::util::toString(e->inPenStatus), blib::math::easyMatrix(e->position*32.0f + e->direction*e->offset*32.0f + glm::vec2(16,16)));
		}

		spriteBatch->end();
	}

	bool PacMan::hasWinner()
	{
		int count = blib::linq::sum<int>(level->levelData, [](std::vector<int> i) { return blib::linq::count(i, [](int ii) { return ii == 2; }); });
		if (count == 0)
			return true;
		int aliveCount = blib::linq::count(players, [](PacManPlayer* p) { return p->alive; });
		if (aliveCount == 0)
			return true;
		if (aliveCount == 1)
			if (blib::linq::max<int, PacManPlayer*>(players, [](PacManPlayer* p) { return p->score; }, [](PacManPlayer* p) { return p; })->alive)
				return true;
		return false;
	}

	std::list<Player*> PacMan::getWinners()
	{
		int maxScore = blib::linq::max<int>(players, [](PacManPlayer* p) { return p->score; });
		return blib::linq::where<std::list<Player*> >(players, [maxScore](PacManPlayer* p) { return p->score == maxScore; });
	}

	blib::Texture* PacMan::getTitleImage()
	{
		return NULL;
	}

}