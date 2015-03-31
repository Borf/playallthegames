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
	gunshots.clear();

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

		if (p->joystick.a == 1)
			p->power += elapsedTime*25;
		if (p->power > 100)
			p->power = 100;

		glm::vec2 oldPosition = p->position;
		if (p->joystick.a == 0)
			p->position += 5.0f * p->joystick.leftStick * 60.0f*elapsedTime;
		else
			p->position += 5.0f * p->joystick.leftStick * 30.0f*elapsedTime;

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
			if(len < 50)
			{
				diff /= len;
				pp->position -= 0.5f * (len-50) * diff;
				p->position += 0.5f * (len-50) * diff;
			}
		}
		if (!screenRect.contains(p->position))
			p->position = oldPosition;

		glm::vec2 shootPos = p->position + 25.0f*blib::util::fromAngle(p->rotation);
		if (p->joystick.a == 0 && p->prevJoystick.a == 1)
		{
			blib::math::Line l(shootPos, shootPos + 2000.0f*blib::util::fromAngle(p->rotation));
			gunshots.push_back(std::pair<glm::vec3, glm::vec4>(glm::vec3(shootPos, p->rotation), glm::vec4(glm::vec3(p->participant->color), p->power / 100.0f)));
			for (auto pp : players)
			{
				if (pp == p || !pp->alive)
					continue;
				if (glm::distance(l.project(pp->position), pp->position) < 50)
					pp->health -= p->power;
			}
			p->power = 0;
		};
	}

	for (int i = 0; i < (int)gunshots.size(); i++)
	{
		gunshots[i].second.a -= (float)elapsedTime;
		if (gunshots[i].second.a < 0)
		{
			gunshots.erase(gunshots.begin() + i);
			i--;
		}
	}
}

void ShowDown::draw()
{
	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(backSprite, glm::mat4());

	for(auto p : players) 
	{ 
		if (!p->alive)
			continue;


		spriteBatch->draw(playerSprite, blib::math::easyMatrix(p->position, glm::degrees(p->rotation), 0.5f), playerSprite->center, blib::math::Rectangle(0, 0, 1, 1));
		spriteBatch->draw(playerOverlaySprite, blib::math::easyMatrix(p->position, glm::degrees(p->rotation), 0.5f), playerSprite->center, blib::math::Rectangle(0, 0, 1, 1), p->participant->color);


		
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(p->position - glm::vec2(100, 50), 0, glm::vec2(200, 10)), glm::vec4(0, 0, 0, 1));
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(p->position - glm::vec2(100, 50), 0, glm::vec2(p->health * 2, 10)), glm::mix(glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), p->health / 100));

		if (p->joystick.a == 1)
		{
			glm::vec2 shootPos = p->position + 25.0f*blib::util::fromAngle(p->rotation);
			spriteBatch->draw(whitePixel, blib::math::easyMatrix(shootPos, glm::degrees(p->rotation), glm::vec2(20000, 2)), p->participant->color);
			
			glm::vec2 offset = p->power / 2.0f * blib::util::fromAngle(p->rotation + blib::math::pif/2);
			spriteBatch->draw(whitePixel, blib::math::easyMatrix(shootPos - offset, glm::degrees(p->rotation), glm::vec2(20000, p->power)), glm::vec4(glm::vec3(p->participant->color),0.5f));
		}
	}


	for (std::pair<glm::vec3, glm::vec4>& gunshot : gunshots)
	{
		glm::vec2 offset = gunshot.second.a * 100 / 2.0f * blib::util::fromAngle(gunshot.first.b + blib::math::pif / 2);
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(glm::vec2(gunshot.first) - offset, glm::degrees(gunshot.first.b), glm::vec2(20000, gunshot.second.a*100.0f)), gunshot.second);
	}


	spriteBatch->end();
}

blib::Texture* ShowDown::getTitleImage()
{
	return NULL;
}
