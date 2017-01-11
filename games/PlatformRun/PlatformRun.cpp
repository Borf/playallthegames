#include "PlatformRun.h"
#include "Level.h"

#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>
#include <blib/util/Log.h>


#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>

using blib::util::Log;

namespace platformrun
{

	PlatformRun::PlatformRun()
	{
		controls[ControllerButton::ButtonA] = "Jump";
		controls[ControllerButton::ThumbstickLeft] = "Move";
	}

	std::string PlatformRun::getName()
	{
		return "PlatformRun";
	}

	std::string PlatformRun::getInstructions()
	{
		return "Run right";
	}

	void PlatformRun::loadResources()
	{
		backgroundTexture = resourceManager->getResource<blib::Texture>("assets/games/PlatformRun/background.png");
		backgroundFrontTexture = resourceManager->getResource<blib::Texture>("assets/games/PlatformRun/background_front.png");
		playerSprite = resourceManager->getResource<blib::Texture>("assets/games/PlatformRun/player.png");
	}

	void PlatformRun::start()
	{
		if (level)
			delete level;
		level = new Level(resourceManager);
		scrollOffset = 0;
		scrollSpeed = 100;
	}

	void PlatformRun::update(float elapsedTime)
	{
		scrollSpeed += elapsedTime*10;
		if (scrollSpeed > 450)
			scrollSpeed = 450;
		scrollOffset += elapsedTime * scrollSpeed;
		for (auto p : players)
		{
			if (p->position.x-1850 > scrollOffset)
				scrollOffset = p->position.x - 1850;

		}
		int screens = scrollOffset / (64 * 64);

		for (auto p : players)
		{
			if (!p->alive)
				continue;
			glm::vec2 prevPosition = p->position;

			if (p->speed.y < 0 && p->joystick.a != 0)
				p->speed.y += 0.2f;
			else
				p->speed.y += 0.75f;

			float targetSpeed = 10 * p->joystick.leftStick.x;
			float diff = p->speed.x - targetSpeed;
			if (diff > elapsedTime*50)
				p->speed.x -= elapsedTime*50;
			else if (diff < -elapsedTime*50)
				p->speed.x += elapsedTime*50;
			else
				p->speed.x = targetSpeed;
			



			if (p->position.x < scrollOffset)
				p->position.x = scrollOffset;

			if (p->position.y > 1080)
			{
				p->alive = false;
				continue;
			}
			

			{
				p->position.x += p->speed.x;
				glm::ivec2 tile;
				if (p->speed.x > 0)
					tile = glm::ivec2(glm::floor((p->position.x+ 63) / 64), glm::floor((p->position.y + 63) / 64));
				else
					tile = glm::ivec2(glm::floor((p->position.x) / 64), glm::floor((p->position.y + 63) / 64));

				if (level->blocking(tile.x-screens*64, tile.y))
				{
					p->position.x = prevPosition.x;
					p->speed.x = 0;
				}

				for (auto pp : players)
				{
					if (!pp->alive || pp == p)
						continue;
					if (p->boundingbox().intersect(pp->boundingbox()))
					{
						pp->speed.x = p->position.x - prevPosition.x;
						p->position.x = prevPosition.x;
						p->speed.x = 0;
					}
				}

			}


			p->position.y += p->speed.y;

			glm::ivec2 floorTile1(glm::floor((p->position.x+2) / 64), glm::ceil(p->position.y / 64) + 0);
			glm::ivec2 floorTile2(glm::floor((p->position.x+62) / 64), glm::ceil(p->position.y / 64) + 0);

			if (level->blocking(floorTile1.x - screens * 64, floorTile1.y) ||
				level->blocking(floorTile2.x - screens * 64, floorTile2.y))
			{
				p->position.y = glm::floor(p->position.y / 64) * 64;
				p->speed.y = 0;
				if (p->joystick.a != 0)
				{
					p->speed.y = -10;
				}
			}


			for (auto pp : players)
			{
				if(!pp->alive || pp == p)
					continue;
				if (p->boundingbox().intersect(pp->boundingbox()))
				{
					p->position.y = prevPosition.y;
					if (p->position.y < pp->position.y)
					{
						p->speed.y = -8;
						pp->speed.y = 3;
					}
					else
					{
						pp->speed.y = -8;
						p->speed.y = 3;
					}
				}

			}


					


			if (p->position.x < scrollOffset)
			{
				p->alive = false;
			}


		}



	}

	void PlatformRun::draw()
	{
		spriteBatch->begin();

		float backgroundScale = 1080.0f / backgroundTexture->originalHeight;
		float offset1 = scrollOffset * .50f - (int)(scrollOffset * .50f / (backgroundTexture->originalWidth*backgroundScale)) * (backgroundTexture->originalWidth*backgroundScale);
		float offset2 = scrollOffset * .75f - (int)(scrollOffset * .75f / (backgroundTexture->originalWidth*backgroundScale)) * (backgroundTexture->originalWidth*backgroundScale);

		spriteBatch->draw(backgroundTexture, blib::math::easyMatrix(glm::vec2(-offset1, 0), 0, backgroundScale));
		spriteBatch->draw(backgroundTexture, blib::math::easyMatrix(glm::vec2(-offset1 + backgroundTexture->originalWidth * backgroundScale, 0), 0, backgroundScale));

		spriteBatch->draw(backgroundFrontTexture, blib::math::easyMatrix(glm::vec2(-offset2, 1080 - backgroundFrontTexture->originalHeight*backgroundScale), 0, backgroundScale));
		spriteBatch->draw(backgroundFrontTexture, blib::math::easyMatrix(glm::vec2(-offset2 + backgroundTexture->originalWidth * backgroundScale, 1080 - backgroundFrontTexture->originalHeight*backgroundScale), 0, backgroundScale));


		level->draw(*spriteBatch, scrollOffset);


		for (auto p : players)
		{
			spriteBatch->draw(playerSprite, blib::math::easyMatrix(p->position - glm::vec2(scrollOffset, 0)), p->participant->color);
		}



		spriteBatch->end();


	}

	blib::Texture* PlatformRun::getTitleImage()
	{
		return NULL;
	}

}