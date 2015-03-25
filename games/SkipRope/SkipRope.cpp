#include "SkipRope.h"

#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/Math.h>
#include <blib/Color.h>

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

#define _USE_MATH_DEFINES
#include <math.h>


std::string SkipRope::getName()
{
	return "SkipRope";
}

std::string SkipRope::getInstructions()
{
	return "Jump over the rope";
}

void SkipRope::loadResources()
{
	playerSprite =		resourceManager->getResource<blib::Texture>("assets/games/SkipRope/player.png");
	playerBackSprite =	resourceManager->getResource<blib::Texture>("assets/games/SkipRope/playerback.png");
	backSprite =		resourceManager->getResource<blib::Texture>("assets/games/SkipRope/back.png");
	handSprite =		resourceManager->getResource<blib::Texture>("assets/games/SkipRope/hand.png");
}

void SkipRope::start(Difficulty difficulty)
{
	frame = (float)M_PI;
	speed = 1;
}

void SkipRope::update( float elapsedTime )
{
	speed += 0.002f * elapsedTime * 60;
	frame += speed*3*elapsedTime;

	while(frame > 2 * M_PI)
	{
		frame -= 2 * (float)M_PI;
		jumpCount++;
	}

	for(auto p : players)
	{
		if (!p->alive)
			continue;

		if(p->position.y == 770 && p->joystick.a == 1)
		{
			p->speed = -8;
		}
		p->position.y += p->speed * elapsedTime * 60;
		p->speed += 0.4f * speed * elapsedTime * 60;
		if (p->position.y >= 770)
			p->position.y = 770;

		if(p->position.y > 765 && frame > 0.4 * M_PI && frame < 0.6 * M_PI)
			p->alive = false;
	}
}

void SkipRope::draw()
{
	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(backSprite, glm::mat4());

	spriteBatch->draw(playerSprite, blib::math::easyMatrix(glm::vec2(255, 735),0,glm::vec2(1.6f, 1.6f)), blib::Color::grey);
	
	spriteBatch->draw(handSprite, blib::math::easyMatrix(glm::vec2(328, 782 + 175 * glm::sin(frame) * glm::sin(25 / 750.0f) * (float)M_PI)), blib::Color::grey);
	spriteBatch->end();
	 

	lineBatch->begin(settings->scaleMatrix);
	for(float x = 335; x < 1035; x++)
	{
		float y = glm::min(830.0f, 790 + 175 * glm::sin(frame) * glm::sin(((x-335+25) / (700+50)) * (float)M_PI));
		lineBatch->draw(glm::vec2(x,y), glm::vec2(x,y+5), blib::Color::white);
	}
	lineBatch->end();

	spriteBatch->begin(settings->scaleMatrix);
	for(auto p : players)
		if(p->alive)
			spriteBatch->draw(playerSprite, blib::math::easyMatrix(p->position), p->participant->color);
	spriteBatch->end();

	if(frame > 1.5*M_PI || frame < 0.5*M_PI)
	{

		lineBatch->begin(settings->scaleMatrix);
		for(float x = 335; x < 1035; x++)
		{
			float y = glm::min(830.0f, 790 + 175 * glm::sin(frame) * glm::sin(((x-335+25) / (700+50)) * (float)M_PI));
			lineBatch->draw(glm::vec2(x,y), glm::vec2(x,y+5), blib::Color::white);
		}
		lineBatch->end();
	}

	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(playerBackSprite, blib::math::easyMatrix(glm::vec2(1000, 735),0,glm::vec2(1.6f, 1.6f)), blib::Color::grey);
	spriteBatch->end();
}

blib::Texture* SkipRope::getTitleImage()
{
	return NULL;
}
