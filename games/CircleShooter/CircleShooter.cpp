#include "CircleShooter.h"
#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>

#include <blib/Math.h>
#include <blib/Util.h>

std::string CircleShooter::getName()
{
	return "CircleShooter";
}

std::string CircleShooter::getInstructions()
{
	return "Dodge";
}

void CircleShooter::loadResources()
{
	playerSprite = resourceManager->getResource<blib::Texture>("assets/games/CircleShooter/player.png");
	bulletSprite = resourceManager->getResource<blib::Texture>("assets/games/CircleShooter/bullet.png");
	backSprite = resourceManager->getResource<blib::Texture>("assets/games/CircleShooter/back.png");
	bubbleSprite = resourceManager->getResource<blib::Texture>("assets/games/CircleShooter/bubble.png");
}

void CircleShooter::start(Difficulty difficulty)
{
	backgroundColor = glm::vec4(1, 1, 1, 1);
	shootTimer = 0;
	index = 0;
	bullets.clear();
	spawnTimer = 1.25f;
}

glm::vec2 CircleShooter::getPos(double d) const
{
	return glm::vec2(1920 / 2, 1080 / 2) + glm::vec2(800.0f, 500.0f) * (1 + 0.025f * glm::cos((float)d + gameTime*10.0f)) * blib::util::fromAngle((float)d + 0.75f * gameTime);
}


void CircleShooter::update(float elapsedTime)
{

	for (auto p : players)
	{
		if (!p->alive)
			continue;
		p->position += elapsedTime * 400 * p->joystick.leftStick;
		if (glm::length(p->position - glm::vec2(1920/2, 1080/2)) > 500)
			p->position -= elapsedTime * 400 * p->joystick.leftStick;

		
		for (auto pp : players)
		{
			if (pp != p && pp->alive)
			{
				glm::vec2 diff = p->position - pp->position;
				float len = glm::length(diff);
				if (len < 64)
				{
					diff /= len;
					p->position += 0.5f * (64 - len) * diff;
					pp->position -= 0.5f * (64 - len) * diff;

				}
			}
		}

		for (size_t i = 0; i < bullets.size(); i++)
		{
			if (glm::distance(p->position, bullets[i].position) < 32+16)
			{
				p->alive = false;
				bullets.erase(bullets.begin() + i);
				break;
			}
		}
	}


	spawnTimer *= glm::pow(0.9f, elapsedTime);
	shootTimer += elapsedTime;
	if (shootTimer > spawnTimer)
	{
		shootTimer -= spawnTimer;
		int i = 0;
		for (double d = 0; d < blib::math::pi * 2; d += blib::math::pi / 7)
		{
			if (index == i)
			{
				glm::vec2 pos = getPos(d);

				Bullet b;
				b.position = pos;
				b.inc = 500.0f * glm::normalize(glm::vec2(1920 / 2, 1080 / 2) - pos);

				bullets.push_back(b);
				index = (index + 1) % 14;
				break;
			}
			i++;
		}
	}

	for (Bullet& b : bullets)
		b.position += b.inc*elapsedTime;

	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [elapsedTime](Bullet b)
	{
		return glm::length(b.position - glm::vec2(1920 / 2, 1080 / 2)) < glm::length(b.position + b.inc*elapsedTime - glm::vec2(1920 / 2, 1080 / 2));
	}), bullets.end());
}

void CircleShooter::draw()
{

	lineBatch->begin(settings->scaleMatrix);
	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(backSprite, glm::mat4());
	
	for (double d = 0; d < blib::math::pi * 2; d += blib::math::pi / 7)
	{
		glm::vec2 pos = getPos(d);
		glm::vec2 pos2 = getPos(d + blib::math::pi / 7);

		spriteBatch->draw(bulletSprite, blib::math::easyMatrix(pos, 0, 2), bulletSprite->center);

		for (int x = -5; x <= 5; x++)
		{
			for (int y = -5; y <= 5; y++)
			{
				lineBatch->draw(pos + glm::vec2(x, y), pos2 + glm::vec2(x, y), glm::vec4(0, 0, 0, 1));
			}
		}
	}

	for (auto b : bullets)
	{
		float dist = 1 + glm::length(b.position - glm::vec2(1920 / 2, 1080 / 2)) / 800;

		spriteBatch->draw(bulletSprite, blib::math::easyMatrix(b.position, 0, dist), bulletSprite->center);
	}

	for (auto p : players)
	{
		if (p->alive)
		{
			spriteBatch->draw(playerSprite, blib::math::easyMatrix(p->position), playerSprite->center, p->participant->color);
			spriteBatch->draw(bubbleSprite, blib::math::easyMatrix(p->position), playerSprite->center);
		}
	}


	spriteBatch->end();
	lineBatch->end();
}

blib::Texture* CircleShooter::getTitleImage()
{
	return NULL;
}
