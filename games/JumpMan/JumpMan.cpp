#include "JumpMan.h"

#include "Enemy.h"
#include "BasicEnemy.h"
#include "JumpEnemy.h"
#include "SlowEnemy.h"

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/math/Rectangle.h>
#include <blib/Math.h>
#include <blib/util/Profiler.h>

#include <glm/gtc/matrix_transform.hpp>

namespace jumpman
{
	std::string JumpMan::getName()
	{
		return "JumpMan";
	}

	std::string JumpMan::getInstructions()
	{
		return "Jump over the car";
	}


	void JumpMan::loadResources()
	{
		sky = resourceManager->getResource<blib::Texture>("assets/games/JumpMan/sky.png");
		playerSprite = resourceManager->getResource<blib::Texture>("assets/games/JumpMan/player.png");
		grass = resourceManager->getResource<blib::Texture>("assets/games/JumpMan/grass.png");
		enemySprite = resourceManager->getResource<blib::Texture>("assets/games/JumpMan/enemy.png");
		wheelSprite = resourceManager->getResource<blib::Texture>("assets/games/JumpMan/wheel.png");


		grassHeight = (int)(40 * settings->scale);
	}

	void JumpMan::start(Difficulty difficulty)
	{
		enemy = new BasicEnemy();
		enemy->position = glm::vec2(1920 - 100, 1080 - 320);

		for (size_t i = 0; i < players.size(); i++)
		{
			players[i]->location = i;
			players[i]->position = glm::vec2(50 + i * 72, 1080 - 90);
			players[i]->jumpSpeed = 0;
		}

		speed = 1;

	}

	void JumpMan::update(float elapsedTime)
	{
		float adjustedSpeed = (1 + (speed - 1) / 100.0f);


		blib::math::Rectangle enemyRect1 = blib::math::Rectangle(enemy->position.x, enemy->position.y + 200, (float)enemySprite->width, 100.0f);
		blib::math::Rectangle enemyRect2 = blib::math::Rectangle(enemy->position.x + 75, enemy->position.y, 150.0f, (float)enemySprite->height);

		for (auto p : players)
		{
			if (!p->alive)
				continue;
			if ((p->position.x - 50) / 72.0f > p->location)
				p->position.x -= 8 * elapsedTime * 60;
			else if ((p->position.x - 50) / 72.0f < p->location)
				p->position.x += 8 * elapsedTime * 60;

			if (p->joystick.a == 1 && p->position.y == 1080 - 90)
				p->jumpSpeed = -30;

			p->jumpSpeed += 0.8f * adjustedSpeed * elapsedTime * 60;
			p->position.y += p->jumpSpeed * adjustedSpeed * elapsedTime * 60;
			if (p->position.y > 1080 - 90)
				p->position.y = 1080 - 90.0f;

			blib::math::Rectangle rect(p->position, (float)playerSprite->width, (float)playerSprite->height);
			if (rect.intersect(enemyRect1) || rect.intersect(enemyRect2))
			{
				p->alive = false;
				std::for_each(players.begin(), players.end(), [p](JumpManPlayer* pp) { if (pp->location > p->location) { pp->location--; } });
			}

		}

		if (enemy->position.x < -400)
		{
			speed += 0.5f;
			double d = blib::math::randomDouble();

			if (d < 0.33333)
				enemy = new BasicEnemy();
			else if (d < 0.66666)
				enemy = new JumpEnemy();
			else
				enemy = new SlowEnemy();
			enemy->position = glm::vec2(1920 - 100, 1080 - 320);


			int aliveCount = blib::linq::count(players, [](JumpManPlayer* pp) { return pp->alive; });
			blib::linq::foreach(players, [aliveCount](JumpManPlayer* p) { p->location = (p->location + 1) % aliveCount; });
		}

		enemy->update(adjustedSpeed * elapsedTime * 60);
	}

	void JumpMan::draw()
	{
		spriteBatch->begin(settings->scaleMatrix);

		spriteBatch->draw(sky, glm::mat4());

		spriteBatch->draw(grass, glm::translate(glm::mat4(), glm::vec3(0, 1060 - grassHeight, 0)));


		for (auto p : players)
			if (p->alive)
				spriteBatch->draw(playerSprite, glm::translate(glm::mat4(), glm::vec3(p->position, 0)), glm::vec2(0, 0), blib::math::Rectangle(0, 0, 1, 1), p->participant->color);

		spriteBatch->draw(enemySprite, glm::translate(glm::mat4(), glm::vec3(enemy->position, 0)));

		spriteBatch->draw(wheelSprite, glm::rotate(glm::translate(glm::mat4(), glm::vec3(enemy->position + glm::vec2(85, 270), 0)), (float)blib::util::Profiler::getAppTime()*-250, glm::vec3(0, 0, 1)), wheelSprite->center);
		spriteBatch->draw(wheelSprite, glm::rotate(glm::translate(glm::mat4(), glm::vec3(enemy->position + glm::vec2(305, 270), 0)), (float)blib::util::Profiler::getAppTime()*-250, glm::vec3(0, 0, 1)), wheelSprite->center);
		spriteBatch->end();

	}

	blib::Texture* JumpMan::getTitleImage()
	{
		return NULL;
	}
}