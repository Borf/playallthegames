#include "Pusher.h"
#include "PusherEnemy.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>

#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

Pusher::Pusher()
{
    controls[ControllerButton::ThumbstickLeft] = "Move";
    for (int i = 0; i < 11; i++) filledLanes.push_back(false);
}

std::string Pusher::getName()
{
    return "Pusher";
}

std::string Pusher::getInstructions()
{
    return "Be the last one alive";
}

void Pusher::update(float elapsedTime)
{
    for (auto p : players)
    {
        if (!p->alive)
            continue;
        p->position += elapsedTime * 400 * p->joystick.leftStick;
        if (p->position.x < 0) p->position.x = 0;
        if (p->position.x > 1900) p->position.x = 1900;
        if (p->position.y < 0) p->position.y = 0;
        if (p->position.y > 1016) p->position.y = 1016;


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

                    if (pp->position.x < 0 || pp->position.x > 1900 || pp->position.y < 0 || pp->position.y > 1016)
                    {
                        p->position -= elapsedTime * 400 * p->joystick.leftStick;
                        pp->position -= elapsedTime * 400 * p->joystick.leftStick;
                    }
                }
            }
        }

		for (auto enemy : enemies)
		{
			if (p->hitbox().intersect(enemy->hitbox()))
				p->alive = false;
		}
    }

    enemySpawnTime -= elapsedTime;
    if (enemySpawnTime <= 0)
    {
        int index = 0;
        for (auto b : filledLanes)
        {
            if (!b)
            {
                int x;
                if (!enemySpawnedLeft)
                { 
                    x = 0;
                    enemySpawnedLeft = true;
                }
                else
                {
                    x = 1920;
                    enemySpawnedLeft = false;
                }
                enemies.push_back(new PusherEnemy(glm::vec2(x, 100 * index), enemySpawnedLeft, &speed));

                b = true;
                enemyCount++;
                enemySpawnTime = 0.5f;
            }
            index++;
        }
    }
	if (!enemies.empty())
	{
		for (int i = 0; i < enemyCount; i++)
		{
			enemies[i]->update(elapsedTime);
			if (enemies[i]->isComingFromLeft)
			{
				if (enemies[i]->position.x > 2120)
				{
					filledLanes[(int)(enemies[i]->position.y) / 100] = false;
					enemies.erase(enemies.begin() + i);
					enemyCount--;
					i--;
				}
			}

			else
			{
				if (enemies[i]->position.x < -250)
				{
					filledLanes[(int)(enemies[i]->position.y) / 100] = false;
					enemies.erase(enemies.begin() + i);
					enemyCount--;
					i--;
				}
			}
		}
	}
	
	updateSpeedTimer -= elapsedTime;
	if (updateSpeedTimer <= 0)
	{
		speed += 20.0f;
		updateSpeedTimer = 1.0f;
	}
}

void Pusher::start()
{
    backgroundColor = glm::vec4(1, 1, 1, 1);

	for (auto p : players)
		p->alive = true;
	if (!enemies.empty())
	{
		enemies.clear();
		enemyCount = 0;
	}

	for(size_t i = 0; i < filledLanes.size(); i++)
		filledLanes[i] = false;

	speed = 150.0f;
	updateSpeedTimer = 5.0f;
}

void Pusher::draw()
{
    spriteBatch->begin(settings->scaleMatrix);
    spriteBatch->draw(levelSprite, glm::mat4());

    for (auto p : players)
    {
        if (p->alive)
        {
            spriteBatch->draw(playerSprite, blib::math::easyMatrix(p->position), playerSprite->center, p->participant->color);
        }
    }

    for (auto e : enemies)
    {
		if (e->isComingFromLeft) carSprite->originalWidth = (int)(carSprite->originalHeight * -1.5);
		else carSprite->originalWidth = (int)(carSprite->originalHeight * 1.5);
        spriteBatch->draw(carSprite, blib::math::easyMatrix(glm::vec2(e->position)));
    }

	
    spriteBatch->end();
}

void Pusher::loadResources()
{
    playerSprite = resourceManager->getResource<blib::Texture>("assets/games/Pusher/player.png");
    levelSprite = resourceManager->getResource<blib::Texture>("assets/games/Pusher/background.jpg");

    carSprite = resourceManager->getResource<blib::Texture>("assets/games/Pusher/car.png");
    truckSprite = resourceManager->getResource<blib::Texture>("assets/games/Pusher/truck.png");

    carSprite->originalHeight = carLength;
}

blib::Texture* Pusher::getTitleImage()
{
    return NULL;
}
