#include "VacuumSucker.h"

#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>

std::string VacuumSucker::getName()
{
	return "VacuumSucker";
}

std::string VacuumSucker::getInstructions()
{
	return "Suck up all the papers";
}

void VacuumSucker::loadResources()
{
	backSprite = resourceManager->getResource<blib::Texture>("assets/games/VacuumSucker/back.png");
	playerSprite = resourceManager->getResource<blib::Texture>("assets/games/VacuumSucker/vacuum.png");
	trashSprite = resourceManager->getResource<blib::Texture>("assets/games/VacuumSucker/trash.png");
	font = resourceManager->getResource<blib::Font>("menu");

}

void VacuumSucker::start(Difficulty difficulty)
{
	trashCount = 2;
	turningFactor = 2;
	trash.clear();

	maxPlayerScore = 10;

	for(auto p : players)
	{
		p->position = glm::vec2(1920/2, 1080/2) + 400.0f * blib::util::fromAngle(p->index/(float) players.size()*2*(float)M_PI);
		p->rotation = 0;
	}


}

void VacuumSucker::update( float elapsedTime )
{
	blib::math::Rectangle screenRect(0,0,1920, 1080);
	for(auto p : players)
	{
		if(p->joystick.a == 1)
		{
			glm::vec2 oldPosition = p->position;
			p->position += 5.0f * blib::util::fromAngle(p->rotation) * 60.0f*elapsedTime;
			bool collision = false;
			for(auto pp : players)
			{
				if(p == pp)
					continue;
				if(glm::length(pp->position - p->position) < 100)
				{
					collision = true;
					pp->rotation += (float)M_PI;
				}
			}
			if (!screenRect.contains(p->position))
				collision = true;

			if(collision)
			{
				p->position = oldPosition;
				p->rotation += (float) M_PI;
			}
		}
		else
		{
			p->rotation += 0.04f*turningFactor * 60 * elapsedTime;
		}


		glm::vec2 suckPos = p->position + 50.0f*blib::util::fromAngle(p->rotation);

		if(blib::linq::any(trash, [p] (glm::vec2 t) { return glm::length(t - p->position) < 64; } ))
		{
			p->score += blib::linq::count(trash, [p] (glm::vec2 t) { return glm::length(t - p->position) < 64; });
			trash = blib::linq::where(trash, [p] (glm::vec2 t) { return glm::length(t - p->position) >= 64; });
		}
	}

	while ((int)trash.size() < trashCount)
	{
		glm::vec2 v(0,0);
		while (!blib::linq::any(players, [this, &v] (VacuumSuckerPlayer* p) { return glm::length(p->position - (v = glm::vec2(blib::math::randomFloat(100.0f, 1920 - 200.0f), blib::math::randomFloat(100.0f, 1080 - 200.0f)))) > 600.0f; })) {}
		trash.push_back(v);
	}
}

void VacuumSucker::draw()
{
	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(backSprite, glm::mat4());

	for(auto t : trash) { spriteBatch->draw(trashSprite, blib::math::easyMatrix(t), trashSprite->center); };
	for(auto p : players) { spriteBatch->draw(playerSprite, glm::rotate(glm::translate(glm::mat4(), glm::vec3(p->position, 0)), glm::degrees(p->rotation), glm::vec3(0,0,1)), playerSprite->center, blib::math::Rectangle(0,0,1,1), p->participant->color);}
	for (size_t i = 0; i < players.size(); i++)
		spriteBatch->draw(font, blib::util::toString(players[i]->score), blib::math::easyMatrix(glm::vec2(10, 64 * i), 0, 1), players[i]->participant->color);

	spriteBatch->end();
}

blib::Texture* VacuumSucker::getTitleImage()
{
	return NULL;
}
