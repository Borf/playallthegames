#include "Battle.h"

#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/SpriteBatch.h>
#include <blib/Math.h>
#include <blib/Util.h>
#include <blib/Font.h>
#include <blib/Color.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#include "BasicBattleLevel.h"
#include "BattleEnemy.h"

#include <glm/gtc/matrix_transform.hpp>

std::string Battle::getName()
{
	return "Battle";
}

std::string Battle::getInstructions()
{
	return "Jump on other people";
}

void Battle::loadResources()
{
	levelTexture = resourceManager->getResource<blib::SpriteSheet>("assets/games/battle/level");
	playerTexture = resourceManager->getResource<blib::Texture>("assets/games/battle/player.png");
	enemyTexture = resourceManager->getResource<blib::Texture>("assets/games/battle/enemy.png");
	enemyMaskTexture = resourceManager->getResource<blib::Texture>("assets/games/battle/enemyMask.png");
	font = resourceManager->getResource<blib::Font>("lindsey");
}

void Battle::start(Difficulty difficulty)
{
	maxPlayerScore = 20;
	level = new BasicBattleLevel(levelTexture);
	auto spawns = level->getSpawns();
	int i = 0;
	for(auto p : players)
	{
		p->position = spawns[p->index % spawns.size()];
	}
	blib::linq::deleteall(enemies);
}

void Battle::update( float elapsedTime )
{
	std::vector<BattleCharacter*> allCharacters = blib::linq::merge<std::vector<BattleCharacter*> >(players, enemies);

	for(auto p : players)
	{
		if (!p->alive)
		{
			p->respawnTime -= elapsedTime;
			if(p->respawnTime < 0)
			{
				auto spawns = level->getSpawns();
				double furthest = -1;
				int furthestSpawnIndex = -1;

				for (size_t i = 0; i < spawns.size(); i++)
				{
					double minDist = 0;
					minDist = blib::linq::min<double>(blib::linq::where(players, [] (BattlePlayer* p) { return p->alive; }), [spawns,i] (BattlePlayer* p) { return glm::length(p->position - glm::dvec2(spawns[i])); },0);
					if(minDist > furthest)
					{
						furthest = minDist;
						furthestSpawnIndex = i;
					}
				}
				p->position = spawns[furthestSpawnIndex];
				p->alive = true;
			}
			else
				continue;
		}

		p->directionalStick = p->joystick.leftStick;
		p->wantsToJump = p->joystick.a == 1;
		p->updateMovement(level, allCharacters, elapsedTime);
	}
	for (auto e : enemies)
	{
		e->updateMovement(level, allCharacters, elapsedTime);
	}
	enemies = blib::linq::where(enemies, [] (BattleEnemy* e) { return e->alive; });

	if (blib::math::randomDouble() < 0.0025f * elapsedTime * 60)
	{
		std::pair<glm::vec2,glm::vec2> spawn = level->enemySpawns[rand()%level->enemySpawns.size()];

		enemies.push_back(new BattleEnemy(spawn.first, spawn.second));
	}

}

void Battle::draw()
{
	spriteBatch->begin(settings->scaleMatrix);
	level->draw(spriteBatch);


	for (auto p : blib::linq::where(players, [](BattlePlayer* p ) { return p->alive; }))
	{
		spriteBatch->draw(playerTexture, blib::math::easyMatrix(glm::vec2(p->position)), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), p->participant->color);
		spriteBatch->draw(font, blib::util::toString(p->score), blib::math::easyMatrix(glm::vec2(p->position) + glm::vec2(0,-64)), p->participant->color);
	}
	for (auto e : enemies)
	{
		if (e->alive)
		{
			glm::mat4 matrix = blib::math::easyMatrix(glm::vec2(e->position));
			if(e->upsidedown)
			{
				matrix = glm::scale(matrix, glm::vec3(1,-1,1));
				matrix = glm::translate(matrix, glm::vec3(0,-enemyTexture->height,0));
			}

			spriteBatch->draw(enemyTexture, matrix);
			if(e->taggedPlayer != NULL)
				spriteBatch->draw(enemyMaskTexture, matrix, glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), e->taggedPlayer->participant->color);
		}
	}

	spriteBatch->end();
}

blib::Texture* Battle::getTitleImage()
{
	return NULL;
}
