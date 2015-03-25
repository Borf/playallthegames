#include "FallDown.h"

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/util/Profiler.h>
#include <blib/Math.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#include <glm/gtc/matrix_transform.hpp>

std::string FallDown::getName()
{
	return "FallDown";
}

std::string FallDown::getInstructions()
{
	return "Fall down to the floor";
}

void FallDown::loadResources()
{
	playerSprite = resourceManager->getResource<blib::Texture>("assets/games/FallDown/player.png");
	blockSprite = resourceManager->getResource<blib::Texture>("assets/games/FallDown/block.png");
	bladeSprite = resourceManager->getResource<blib::Texture>("assets/games/FallDown/blade.png");
	bladeBloodySprite = resourceManager->getResource<blib::Texture>("assets/games/FallDown/bladebloody.png");
	backSprite = resourceManager->getResource<blib::Texture>("assets/games/FallDown/back.png");
}

void FallDown::start(Difficulty difficulty)
{
	speed = 1;
	levelWidth = 1920/128;
	levelHeight = 1080/128 + 2;
	if (levelHeight%2 == 1)
		levelHeight++;
	level.clear();
	for (int i = 0; i < levelHeight; i++)
	{
		std::vector<bool> line;
		for (int ii = 0; ii < levelWidth; ii++)
			line.push_back(i%2 == 0);

		line[0] = true;
		line[line.size() - 1] = true;

		if (i%2 == 0)
			line[1 + rand()%(line.size() - 2)] = false;
		level.push_back(line);
	}
	generateEmptyLine = false;

	offset = 0;
	for(auto p : players) { p->position = glm::vec2(128 + 64*p->index, 64 + (levelHeight - 5) * 128); };
	touchedBlades = std::vector<bool>(30, false);
	scrollBack = 0;

}

void FallDown::update( float elapsedTime )
{
	speed = speed + 0.002f * elapsedTime * 60;



	float offsetDiff = glm::min(125.0f, glm::pow(speed * 6, 0.6f) * 2) * elapsedTime * 60;
	offset += offsetDiff;

	if (offset > 128)
	{
		offset -= 128;

		std::vector<bool> line;
		for (int ii = 0; ii < levelWidth; ii++)
			line.push_back(!generateEmptyLine);

		if (!generateEmptyLine)
		{
			line[1 + rand()%(line.size() - 2)] = false;
			if(blib::math::randomDouble() < 0.5)
				line[1 + rand()%(line.size() - 2)] = false;
		}
		line[0] = true;
		line[line.size() - 1] = true;

		level.erase(level.begin());
		level.push_back(line);

		generateEmptyLine = !generateEmptyLine;
	}


	scrollBack -= 0.5f*offsetDiff;
	while (scrollBack < 0)
		scrollBack += backSprite->originalHeight;


	for(auto p : players)
	{
		if (!p->alive)
		{
			p->position.y -= 3 * glm::pow(speed*6, 0.6f);
			continue;
		}

		float fac = 8*glm::pow(speed, 0.6f);
		fac = glm::sqrt(fac);
		for (int ii = 0; ii < glm::ceil(fac*fac); ii++)
		{

			p->position.y += settings->scale * (float)fac / 4 * elapsedTime * 60;
			if (p->position.y > 1080 - playerSprite->originalHeight)
				p->position.y = (float)(1080 - playerSprite->originalHeight);
			blib::math::Rectangle playerRect(glm::floor(p->position), playerSprite->originalWidth, playerSprite->originalHeight);
			if (hasCollision(playerRect))
				p->position.y = 128 * glm::round((p->position.y + offset - 64) / 128) + 64 - offset;


			float oldPos = p->position.x;
			p->position.x += fac * p->joystick.leftStick.x * elapsedTime * 60;
			playerRect = blib::math::Rectangle(glm::floor(p->position), playerSprite->originalWidth, playerSprite->originalHeight);
			if (hasCollision(playerRect))
				p->position.x = 64 * glm::round(oldPos / 64);
			oldPos = p->position.x;

			p->position.x += fac * p->joystick.rightStick.x * 0.5f * elapsedTime * 60;
			playerRect = blib::math::Rectangle(glm::floor(p->position), playerSprite->originalWidth, playerSprite->originalHeight);
			if (hasCollision(playerRect))
				p->position.x = 64 * glm::round(oldPos / 64);
		}

		if (p->position.y < 32 || p->position.x < 32 || p->position.x > 1920-32)
		{
			for (int ii = glm::max(0, (int)(p->position.x / 100) - 1); ii <= glm::min(29, (int)(p->position.x / 100) + 1); ii++)
				touchedBlades[ii] = true;
			p->alive = false;
		}
	}

}


bool FallDown::hasCollision(const blib::math::Rectangle &r)
{
	for (size_t i = 0; i < level.size(); i++)
	{
		for (size_t ii = 0; ii < level[i].size(); ii++)
		{
			if (level[i][ii])
			{
				blib::math::Rectangle r2(glm::vec2(128 * ii, 128 * i - (int)offset), 128, 128);
				if (r2.intersect(r))
					return true;
			}
		}
	}
	return false;
}


void FallDown::draw()
{
	spriteBatch->begin(settings->scaleMatrix);

	for (int x = 0; x < 1920; x += backSprite->originalWidth)
		for (int y = -(int)scrollBack; y < 1080 + scrollBack; y += backSprite->originalHeight)
			spriteBatch->draw(backSprite, glm::translate(glm::mat4(), glm::vec3(x,y,0)));



	for (size_t i = 0; i < level.size(); i++)
	{
		for (size_t ii = 0; ii < level[i].size(); ii++)
		{
			if (level[i][ii])
				spriteBatch->draw(blockSprite, glm::translate(glm::mat4(), glm::vec3(128 * ii, 128 * i - offset, 0)));
		}
	}

	for (auto p : players)
		spriteBatch->draw(playerSprite, glm::translate(glm::mat4(), glm::vec3(p->position,0)), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), p->participant->color);


	for (int i = 0; i < 30; i++)
	{
		spriteBatch->draw(touchedBlades[i]?bladeBloodySprite:bladeSprite, 
			glm::rotate(glm::translate(glm::mat4(), glm::vec3(i * 100 + 50, -20,0)), glm::degrees(i + (float)blib::util::Profiler::getAppTime() * 10 * glm::pow(10+glm::sin((float)i), 0.4f) ), glm::vec3(0,0,1)), bladeSprite->center);
	}
	spriteBatch->end();

}

blib::Texture* FallDown::getTitleImage()
{
	return NULL;
}
