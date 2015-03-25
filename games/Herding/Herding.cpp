#include "Herding.h"
#include "Sheep.h"

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/Math.h>
#include <blib/linq.h>
#include <blib/Font.h>
#include <blib/Util.h>

#include <glm/gtc/matrix_transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

std::string Herding::getName()
{
	return "Herding";
}

std::string Herding::getInstructions()
{
	return "Put the sheep with your flowers";
}

void Herding::loadResources()
{
	background = resourceManager->getResource<blib::Texture>("assets/games/Herding/back.png");
	corner = resourceManager->getResource<blib::Texture>("assets/games/Herding/corner.png");
	sheepSprite = resourceManager->getResource<blib::Texture>("assets/games/Herding/sheep.png");
	playerSprite = resourceManager->getResource<blib::Texture>("assets/games/Herding/player.png");
	font = resourceManager->getResource<blib::Font>("lindsey");
}

void Herding::start(Difficulty difficulty)
{
	totalScore = 20;

	glm::vec2 positions[] = { glm::vec2(560, 260), glm::vec2(1270, 260), glm::vec2(560, 830), glm::vec2(1270, 830) }; //TODO: this should be done in in another way?
	for(size_t i = 0; i < players.size(); i++)
	{
		players[i]->basePosition.x = i % 2 == 0 ? 0.0f : 1920.0f;
		players[i]->basePosition.y = i / 2 == 0 ? 0.0f : 1080.0f;
		players[i]->position = positions[i];
	}

	std::for_each(sheep.begin(), sheep.end(), [] (Sheep* sheep) { delete sheep; });
	sheep.clear();

	while(sheep.size() < 20)
	{
		glm::vec2 newPos = glm::vec2(700 + (float)blib::math::randomDouble() * 400, 400 + (float)blib::math::randomDouble() * 400);
		bool hasCollision = false;
		for (size_t ii = 0; ii < sheep.size(); ii++)
			if(glm::length(sheep[ii]->position - newPos) < (sheepSprite->height/16) * 0.5)
				hasCollision = true;
		if (!hasCollision)
		{
			Sheep* s = new Sheep();
			s->position = newPos;
			s->direction = 0;
			s->moveFrame = 0;
			sheep.push_back(s);
		}
	}
	std::sort(sheep.begin(), sheep.end(), [] (Sheep* a, Sheep* b) { return a->position.y < b->position.y; });

}

blib::Texture* Herding::getTitleImage()
{
	return NULL;
}

void Herding::update( float elapsedTime )
{
	for(auto p : players)
	{
		p->position += elapsedTime * 600 * p->joystick.leftStick;
		p->position = glm::vec2(glm::min(1920.0f, glm::max(0.0f, p->position.x)), glm::min(1080.0f, glm::max(0.0f, p->position.y)));
	}

	std::vector<glm::vec2> oldPositions = blib::linq::select<std::vector<glm::vec2> >(sheep, [] (Sheep* s) { return s->position; });
	for(auto s : sheep)
	{
		for(auto player : players)
		{
			glm::vec2 diff = player->position - s->position;
			float len = glm::length(diff);
			len = 200 - len;

			if (glm::length(diff) < 200 && glm::length(diff) > 0)
			{
				diff = glm::normalize(diff);
				s->position -= len * diff;
				s->position = glm::vec2(glm::min(1920.0f, glm::max(0.0f, s->position.x)), glm::min(1080.0f, glm::max(0.0f, s->position.y)));
				checkCollision(s);
			}
			s->position = glm::vec2(glm::min(1920.0f, glm::max(0.0f, s->position.x)), glm::min(1080.0f, glm::max(0.0f, s->position.y)));
			checkCollision(s);
		}
		glm::vec2 direction = glm::vec2(0,0);
		for(auto ss : sheep)
		{
			if (s == ss)
				continue;
			glm::vec2 diff = ss->position - s->position;
			direction += diff;
		}

		direction = glm::normalize(direction);
		if (glm::length(direction) > 0.1)
		{
			direction = glm::normalize(direction);
			glm::vec2 oldPos = s->position;
			s->position += 60 * elapsedTime * direction * 2.0f;

			if (checkCollision2(s))
				s->position = oldPos;
		}
	}

	for (size_t i = 0; i < sheep.size(); i++)
	{
		glm::vec2 diff = oldPositions[i] - sheep[i]->position;
		if (glm::length(diff) > 0.01)
		{
			sheep[i]->direction = 0.9f * sheep[i]->direction + 0.1f * (float)atan2(diff.y, diff.x);
			sheep[i]->moveFrame += 0.1f + (float)elapsedTime * 5 * glm::length(diff);
			while (sheep[i]->moveFrame > 4)
				sheep[i]->moveFrame -= 4;
		}
		else
			sheep[i]->moveFrame = 0;
	}

	for (int i = sheep.size() - 1; i >= 0; i--)
	{
		HerdingPlayer* taken = NULL;
		for (auto player : players)
			if (glm::length(sheep[i]->position - player->basePosition) < 250)
				taken = player;

		if (taken != NULL)
		{
			taken->score++;
			sheep.erase(sheep.begin() + i);
		}
	}
	std::sort(sheep.begin(), sheep.end(), [] (Sheep* a, Sheep* b) { return a->position.y < b->position.y; });
}



void Herding::checkCollision(Sheep* movedSheep)
{
	for(auto  s : sheep)
	{
		if (s == movedSheep)
			continue;
		glm::vec2 diff = s->position - movedSheep->position;
		float len = glm::length(diff);
		if (len < sheepSprite->width/4/2)
		{
			diff = glm::normalize(diff);
			s->position -= diff * (len - sheepSprite->width/4/2 - 1);
			s->position = glm::vec2(glm::min(1920.0f, glm::max(0.0f, s->position.x)), glm::min(1080.0f, glm::max(0.0f, s->position.y)));
			checkCollision(s);
		}
	}
}

bool Herding::checkCollision2(Sheep* movedSheep)
{
	for(auto s : sheep)
	{
		if (s == movedSheep)
			continue;
		glm::vec2 diff = s->position - movedSheep->position;
		float len = glm::length(diff);
		if (len < sheepSprite->width/4/2)
			return true;
	}
	return false;
}

void Herding::draw()
{
	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(background, glm::mat4());

	spriteBatch->draw(corner, glm::mat4(), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), players[0]->participant->color);
	if (players.size() > 1)
		spriteBatch->draw(corner, glm::scale(glm::translate(glm::mat4(), glm::vec3(1920, 0, 0)), glm::vec3(-1, 1, 1)), glm::vec2(0, 0), blib::math::Rectangle(0, 0, 1, 1), players[1]->participant->color);

	if (players.size() > 2)
		spriteBatch->draw(corner, glm::scale(glm::translate(glm::mat4(), glm::vec3(0, 1080, 0)), glm::vec3(1, -1, 1)), glm::vec2(0, 0), blib::math::Rectangle(0, 0, 1, 1), players[2]->participant->color);
	if (players.size() > 3)
		spriteBatch->draw(corner, glm::scale(glm::translate(glm::mat4(), glm::vec3(1920, 1080, 0)), glm::vec3(-1,-1,1)), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), players[3]->participant->color);


	spriteBatch->draw(font, blib::util::toString(players[0]->score), glm::translate(glm::mat4(), glm::vec3(11,11,0)), glm::vec4(0,0,0,1));
	spriteBatch->draw(font, blib::util::toString(players[0]->score), glm::translate(glm::mat4(), glm::vec3(10,10,0)), glm::vec4(1,1,1,1));

	if (players.size() > 1)
	{
		spriteBatch->draw(font, blib::util::toString(players[1]->score), glm::translate(glm::mat4(), glm::vec3(1851, 11, 0)), glm::vec4(0, 0, 0, 1));
		spriteBatch->draw(font, blib::util::toString(players[1]->score), glm::translate(glm::mat4(), glm::vec3(1850, 10, 0)), glm::vec4(1, 1, 1, 1));
	}

	if (players.size() > 2)
	{
		spriteBatch->draw(font, blib::util::toString(players[2]->score), glm::translate(glm::mat4(), glm::vec3(11, 1001, 0)), glm::vec4(0, 0, 0, 1));
		spriteBatch->draw(font, blib::util::toString(players[2]->score), glm::translate(glm::mat4(), glm::vec3(10, 1000, 0)), glm::vec4(1, 1, 1, 1));
	}

	if (players.size() > 3)
	{
		spriteBatch->draw(font, blib::util::toString(players[3]->score), glm::translate(glm::mat4(), glm::vec3(1851, 1001, 0)), glm::vec4(0, 0, 0, 1));
		spriteBatch->draw(font, blib::util::toString(players[3]->score), glm::translate(glm::mat4(), glm::vec3(1850, 1000, 0)), glm::vec4(1, 1, 1, 1));
	}

	for(auto s : sheep)
	{
		float dir = s->direction / (float)(2*M_PI);
		while(dir > 1)
			dir--;
		while(dir < 0)
			dir++;
		dir = floor(dir*16);

		spriteBatch->draw(sheepSprite, glm::translate(glm::mat4(), glm::vec3(s->position, 0)), sheepSprite->center / glm::vec2(4, 16), blib::math::Rectangle((int)s->moveFrame*0.25f,dir/16,0.25f,1/16.0f));
	}

	for(auto p : players)
		spriteBatch->draw(playerSprite, glm::translate(glm::mat4(), glm::vec3(p->position,0)), playerSprite->center, blib::math::Rectangle(0,0,1,1), p->participant->color);

	spriteBatch->end();


}
