#include "BugSwat.h"
#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"
#include "Splat.h"
#include "Enemy.h"
#include "SwirlEnemy.h"
#include "DodgeEnemy.h"


#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/SpriteSheet.h>
#include <blib/Texture.h>
#include <blib/util/Profiler.h>
#include <blib/Math.h>
#include <blib/Util.h>

#include <glm/gtc/matrix_transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

namespace bugswat
{
	std::string BugSwat::getName()
	{
		return "BugSwat";
	}

	std::string BugSwat::getInstructions()
	{
		return "Swat all the flies";
	}


	void BugSwat::loadResources()
	{
		swatterSprite = resourceManager->getResource<blib::SpriteSheet>("assets/games/BugSwat/swatter");
		back = resourceManager->getResource<blib::Texture>("assets/games/BugSwat/back.png");
		flySprite = resourceManager->getResource<blib::SpriteSheet>("assets/games/BugSwat/fly");
		splatSprite = resourceManager->getResource<blib::Texture>("assets/games/BugSwat/splat.png");
		font = resourceManager->getResource<blib::Font>("menu");
	}

	void BugSwat::start(Difficulty difficulty)
	{
		blib::linq::foreach(players, [this](BugSwatPlayer* p) { p->position = glm::vec2(1920 / 2, 1080 / 2) + 300.0f * blib::util::fromAngle(p->index * ((float)M_PI * 2 / players.size())); });
		blib::linq::deleteall(enemies);
		blib::linq::deleteall(splats);
	}

	void BugSwat::update(float elapsedTime)
	{
		for (auto p : players)
		{
			p->position += (12.0f * p->joystick.leftStick + 15.0f * p->joystick.rightStick) * elapsedTime*60.0f;

			p->position.x = glm::min((float)1920, glm::max(0.0f, p->position.x));
			p->position.y = glm::min((float)1080, glm::max(0.0f, p->position.y));

			if ((p->joystick.a == 1 || p->joystick.r == 1) && !p->swatting)
			{
				p->swatTime = blib::util::Profiler::getAppTime();
				p->swatting = true;
			}
			else if (p->joystick.a == 0 && p->joystick.r == 0)
				p->swatting = false;
		}

		for (auto e : enemies)
		{
			e->update(elapsedTime * 60);
			if (e->alive)
			{
				auto hittingPlayers = blib::linq::where(players, [e](BugSwatPlayer* p) { return glm::length(p->position - e->position) < 55 && p->swatting && (int)floor((blib::util::Profiler::getAppTime() - p->swatTime) * 50) == 3; });
				if (hittingPlayers.size() > 0)
				{
					e->alive = !e->alive;
					blib::linq::foreach(hittingPlayers, [](BugSwatPlayer* p) { p->score++; });
					splats.push_back(new Splat(e->position, (float)(blib::math::randomDouble()*M_PI * 2)));
					//particleEmitters.Add(new FlyBloodEmitter(e.position, elapsedTime));
				}
			}
		}
		enemies = blib::linq::where(enemies, [this](Enemy* e) { return e->onScreen(settings); });

		// Spawn new enemy
		if (blib::math::randomDouble() < 0.03 * elapsedTime * 60)
		{
			Enemy* enemy = NULL;

			int enemyType = rand() % 3;

			if (enemyType == 0)
				enemy = new Enemy();
			if (enemyType == 1)
				enemy = new SwirlEnemy();
			if (enemyType == 2)
				enemy = new DodgeEnemy(this);

			int side = rand() % 4;
			float d = (float)blib::math::randomDouble();
			glm::vec2 target(rand() % 1920, rand() % 1080);

			glm::vec2 positions[] = {
				glm::vec2(d * 1920, 0),
				glm::vec2(d * 1920, 1080),
				glm::vec2(0, d * 1080),
				glm::vec2(1920, d * 1080),
			};

			enemy->position = positions[side];
			enemy->rotation = atan2(target.y - enemy->position.y, target.x - enemy->position.x);
			enemy->speed = 7 + (float)blib::math::randomDouble() * 3;
			enemies.push_back(enemy);
		}
	}

	void BugSwat::draw()
	{
		spriteBatch->begin(settings->scaleMatrix);

		spriteBatch->draw(back, glm::mat4());

		blib::linq::foreach(splats, [this](Splat* s) {
			spriteBatch->draw(splatSprite, s->getMatrix(), splatSprite->center);
		});


		blib::linq::foreach(enemies, [this](Enemy* e) {
			spriteBatch->draw(flySprite->getFrame(e->alive ? (int)(blib::util::Profiler::getAppTime() * 20) % 2 : 2, 0), e->getMatrix(), flySprite->spriteCenter);//, e.position, e.rotation, new Vector2(flySprite.spriteWidth / 2, flySprite.spriteHeight / 2), e.alive ? Color.White : new Color(0.25f, 0.25f, 0.25f)));
		});


		for (auto player : players)
		{
			int frame = 0;
			if (player->swatting)
			{
				double swatTime = blib::util::Profiler::getAppTime() - player->swatTime;
				swatTime *= 50;
				if (swatTime < 8)
				{
					frame = (int)swatTime;
					if (frame >= 4)
						frame = 7 - frame;
				}
			}

			spriteBatch->draw(swatterSprite->getFrame(frame, 0), glm::translate(glm::mat4(), glm::vec3(player->position, 0)), glm::vec2(31, 22), player->participant->color);
		}



		for (size_t i = 0; i < players.size(); i++)
			spriteBatch->draw(font, blib::util::toString(players[i]->score), blib::math::easyMatrix(glm::vec2(10, 64 * i), 0, 1), players[i]->participant->color);


		spriteBatch->end();
	}

	blib::Texture* BugSwat::getTitleImage()
	{
		return NULL;
	}
}