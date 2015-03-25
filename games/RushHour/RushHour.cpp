#include "RushHour.h"

#include <glm/gtc/matrix_transform.hpp>

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/Texture.h>
#include <blib/Math.h>
#include <blib/Color.h>
#include <blib/Util.h>
#include <blib/util/Profiler.h>
#include <blib/ParticleSystem.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

RushHour::RushHour()
{
}


std::string RushHour::getName()
{
	return "RushHour";
}

std::string RushHour::getInstructions()
{
	return "Don't let cars block up 'n stuff";
}

void RushHour::loadResources()
{
	backgroundTexture = resourceManager->getResource<blib::Texture>("assets/games/RushHour/back.png");
	carSprite = resourceManager->getResource<blib::Texture>("assets/games/RushHour/sportscar.png");
	cursorSprite = resourceManager->getResource<blib::Texture>("assets/games/RushHour/crosshair.png");
	whitePixel = resourceManager->getResource<blib::Texture>("assets/textures/whitepixel.png");

	particleSystem = new blib::ParticleSystem(renderer, resourceManager, spriteBatch);
	particleSystem->setTextureFolder("assets/games/RushHour/particles/");
}


static int posY[] = { 210, 260, 310, 360, 720, 770, 820, 870 };

static int posX[] = { 490, 540, 590, 640, 1290, 1340, 1390, 1440};

void RushHour::start(Difficulty difficulty)
{
	lanes.clear();
	lanes.resize(16);

	for (int i = 0; i < 8; i++)
	{
		lanes[i].orientation = Lane::Horizontal;
		lanes[i].direction = (i / 2) % 2 == 1 ? 1 : -1;
	
		lanes[i].spawn = glm::vec2(lanes[i].direction > 0 ? 0 : 1920, posY[i]);
		lanes[i].spawnTime = blib::math::randomFloat(1, 15);
	}
	for (int i = 8; i < 16; i++)
	{
		lanes[i].orientation = Lane::Vertical;
		lanes[i].direction = (i / 2) % 2 == 0 ? 1 : -1;
		lanes[i].spawn = glm::vec2(posX[i-8], lanes[i].direction > 0 ? 0 : 1080);
		lanes[i].spawnTime = blib::math::randomFloat(1, 15);
	}


}


void RushHour::update( float elapsedTime )
{
	particleSystem->update(elapsedTime);

	for (Lane& lane : lanes)
	{
		lane.spawnTime -= elapsedTime;
		if (lane.spawnTime < 0)
		{
			lane.spawnTime = blib::math::randomFloat(5, 15);
			Car c;
			c.ob = false;
			c.pos = -50;
			c.state = Car::Driving;
			c.speed = 1;
			c.deadTimer = 9999;
			lane.push_back(c);
		}


		for (Car& car : lane)
		{
			car.deadTimer -= elapsedTime;
			if (car.state == Car::Stopped)
				continue;


			if ((lane.orientation == Lane::Vertical && car.pos > 1150) ||
				(lane.orientation == Lane::Horizontal && car.pos > 2000))
			{
				car.ob = true;
				continue;
			}

			car.pos += car.getSpeed() * elapsedTime * 100;
			blib::math::Rectangle rect = lane.getRect(car);
			
			bool collides = false;
			for (Lane& l : lanes)
				for (Car& otherCar : l)
					if (otherCar != car)
						if (rect.overlap(l.getRect(otherCar)) > 0)
						{
							collides = true;
							if (l.orientation != lane.orientation)
							{
								
								if (car.deadTimer > 1)
								{
									car.state = Car::Stopped;
									car.deadTimer = 1;
									otherCar.state = Car::Stopped;
									otherCar.deadTimer = 1;
									blib::Emitter* emitter = particleSystem->addEmitter("assets/games/RushHour/particles/explosion.json");
									emitter->position = emitter->prevPosition = lane.getPos(car);
									emitter->life = 0.5;


									for (auto p : players)
										if (p->screenRect.contains(lane.getPos(car)) || p->screenRect.contains(l.getPos(otherCar)))
											p->alive = false;

								}
								break;
							}
						}
			if (collides)
				car.pos -= car.getSpeed() * elapsedTime * 100;

		}

		lane.erase(std::remove_if(lane.begin(), lane.end(), [elapsedTime](Car &c)	{		return c.ob || c.deadTimer < 0; } ), lane.end());
	}


	for (auto p : players)
	{
		if (!p->alive)
			continue;
		p->cursor += p->joystick.leftStick * elapsedTime * 500.0f;
		if (!p->screenRect.contains(p->cursor))
			p->cursor -= p->joystick.leftStick * elapsedTime * 500.0f;

		if (p->joystick.a == 1 || p->joystick.b == 1)
		{
			for (Lane& lane : lanes)
			{
				for (Car& car : lane)
				{
					if (lane.getRect(car).contains(p->cursor))
					{
						car.state = p->joystick.a == 1 ? Car::SpedUp : Car::Stopped;
					}
				}
			}
		}
	}
}

void RushHour::draw()
{

	spriteBatch->begin();

	spriteBatch->draw(backgroundTexture, glm::mat4());

	lineBatch->begin();

	for (int i = 0; i < 16; i++)
	{
		for (size_t ii = 0; ii < lanes[i].size(); ii++)
		{
			float angle = 0;
			if (lanes[i].orientation == Lane::Horizontal)
				angle = lanes[i].direction > 0 ? 0.0f : 180.0f;
			if (lanes[i].orientation == Lane::Vertical)
				angle = lanes[i].direction > 0 ? 90.0f : -90.0f;


			glm::vec4 color = glm::vec4(1, 1, 1, 1);
			if (lanes[i][ii].deadTimer < 0)
				lanes[i][ii].deadTimer = 0;
			if (lanes[i][ii].deadTimer < 1)
				color = glm::vec4(lanes[i][ii].deadTimer, lanes[i][ii].deadTimer, lanes[i][ii].deadTimer, lanes[i][ii].deadTimer * lanes[i][ii].deadTimer);

			spriteBatch->draw(carSprite, blib::math::easyMatrix(lanes[i].getPos(lanes[i][ii]), angle), carSprite->center, color);
	//		lineBatch->draw(lanes[i].getRect(lanes[i][ii]));
		}
	}


	particleSystem->draw(glm::mat4());

	for (auto p : players)
		if (p->alive)
			spriteBatch->draw(cursorSprite, blib::math::easyMatrix(p->cursor, (float)blib::util::Profiler::getAppTime()*100), cursorSprite->center, p->participant->color);


	spriteBatch->end();

	lineBatch->end();


	spriteBatch->begin();
	for (auto p : players)
		if (!p->alive)
			spriteBatch->draw(whitePixel, blib::math::easyMatrix(whitePixel, p->screenRect), glm::vec4(0, 0, 0, 0.5f));
	spriteBatch->end();

}

blib::Texture* RushHour::getTitleImage()
{
	return NULL;
}

glm::vec2 RushHour::Lane::getPos(const Car &c)
{
	float angle = 0;
	if (orientation == Lane::Horizontal)
		angle = direction > 0.0f ? 0.0f : 180.0f;
	if (orientation == Lane::Vertical)
		angle = direction > 0.0f ? 90.0f : -90.0f;

	return spawn + c.pos * blib::util::fromAngle(glm::radians(angle));


}

blib::math::Rectangle RushHour::Lane::getRect(Car& c)
{
	float angle = 0;
	if (orientation == Lane::Horizontal)
		angle = direction > 0 ? 0.0f : 180.0f;
	if (orientation == Lane::Vertical)
		angle = direction > 0 ? 90.0f : -90.0f;

	glm::vec2 center = spawn + c.pos * blib::util::fromAngle(glm::radians(angle));

	if (orientation == Lane::Horizontal)
		return blib::math::Rectangle(center - glm::vec2(40, 20), center + glm::vec2(40, 20));
	else
		return blib::math::Rectangle(center - glm::vec2(20, 40), center + glm::vec2(20, 40));

}
