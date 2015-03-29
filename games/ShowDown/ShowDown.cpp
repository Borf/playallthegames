#include "ShowDown.h"

#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>
#include <blib/math/Line.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>

std::string ShowDown::getName()
{
	return "ShowDown";
}

std::string ShowDown::getInstructions()
{
	return "Shoot other players";
}

void ShowDown::loadResources()
{
	backSprite = resourceManager->getResource<blib::Texture>("assets/games/ShowDown/back.png");
	playerSprite = resourceManager->getResource<blib::Texture>("assets/games/ShowDown/player.png");
	playerOverlaySprite = resourceManager->getResource<blib::Texture>("assets/games/ShowDown/player.overlay.png");
	playerNormalSprite = resourceManager->getResource<blib::Texture>("assets/games/ShowDown/player.normals.png");
	whitePixel = resourceManager->getResource<blib::Texture>("assets/textures/whitepixel.png");
	font = resourceManager->getResource<blib::Font>("menu");

}

void ShowDown::start(Difficulty difficulty)
{

	for(auto p : players)
	{
		p->position = glm::vec2(1920/2, 1080/2) + 400.0f * blib::util::fromAngle(p->index/(float) players.size()*2*(float)M_PI);
		p->rotation = p->index / (float)players.size() * 2 * (float)M_PI;
	}


}

void ShowDown::update( float elapsedTime )
{
	blib::math::Rectangle screenRect(0,0,1920, 1080);
	for(auto p : players)
	{
		if (p->health < 0)
			p->alive = false;
		if (!p->alive)
			continue;

		if (p->joystick.a == 0)
			p->power += elapsedTime*25;
		else
			p->power += elapsedTime*15;
		if (p->power > 100)
			p->power = 100;
		glm::vec2 oldPosition = p->position;
		p->position += 5.0f * p->joystick.leftStick * 60.0f*elapsedTime;

		if (glm::length(p->joystick.leftStick) > 0.1f)
		{
			p->rotation = atan2(p->joystick.leftStick.y, p->joystick.leftStick.x);
		}

		for(auto pp : players)
		{
			if(p == pp || !pp->alive)
				continue;
			glm::vec2 diff = pp->position - p->position;
			float len = glm::length(diff);
			if(len < 100)
			{
				diff /= len;
				pp->position -= 0.5f * (len-100) * diff;
				p->position += 0.5f * (len-100) * diff;
			}
		}
		if (!screenRect.contains(p->position))
			p->position = oldPosition;

		glm::vec2 shootPos = p->position + 50.0f*blib::util::fromAngle(p->rotation);
		if (p->joystick.a)
		{
			blib::math::Line l(shootPos, shootPos + 2000.0f*blib::util::fromAngle(p->rotation));
			for (auto pp : players)
			{
				if (pp == p || !pp->alive)
					continue;
				if (glm::distance(l.project(pp->position), pp->position) < 100)
					pp->health -= p->power;
			}
			p->power = 0;
		};
	}
}

void ShowDown::draw()
{
	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(backSprite, glm::mat4());

	for(auto t : trash) { spriteBatch->draw(trashSprite, blib::math::easyMatrix(t), trashSprite->center); };
	for(auto p : players) 
	{ 
		if (!p->alive)
			continue;
		spriteBatch->draw(playerSprite, glm::rotate(glm::translate(glm::mat4(), glm::vec3(p->position, 0)), glm::degrees(p->rotation), glm::vec3(0, 0, 1)), playerSprite->center, blib::math::Rectangle(0, 0, 1, 1));
		spriteBatch->draw(playerOverlaySprite, glm::rotate(glm::translate(glm::mat4(), glm::vec3(p->position, 0)), glm::degrees(p->rotation), glm::vec3(0, 0, 1)), playerSprite->center, blib::math::Rectangle(0, 0, 1, 1), p->participant->color);


		
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(p->position - glm::vec2(100, 50), 0, glm::vec2(200, 10)), glm::vec4(0, 0, 0, 1));
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(p->position - glm::vec2(100, 50), 0, glm::vec2(p->health * 2, 10)), glm::mix(glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), p->health / 100));

		spriteBatch->draw(whitePixel, blib::math::easyMatrix(p->position - glm::vec2(100, 40), 0, glm::vec2(200, 10)), glm::vec4(0, 0, 0, 1));
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(p->position - glm::vec2(100, 40), 0, glm::vec2(p->power * 2, 10)), glm::mix(glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), p->power / 100));
		if (p->joystick.a == 1)
		{
			glm::vec2 shootPos = p->position + 50.0f*blib::util::fromAngle(p->rotation);
			spriteBatch->draw(whitePixel, blib::math::easyMatrix(shootPos, glm::degrees(p->rotation), glm::vec2(20000, 2)), p->participant->color);
		}
	}

	spriteBatch->end();
}

blib::Texture* ShowDown::getTitleImage()
{
	return NULL;
}
