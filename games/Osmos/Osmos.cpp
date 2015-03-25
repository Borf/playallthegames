#include "Osmos.h"
#include "OsmosPlayer.h"
#include "Sphere.h"
#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Math.h>
#include <blib/linq.h>
#include <blib/Util.h>

#define _USE_MATH_DEFINES
#include <math.h>


std::string Osmos::getName()
{
	return "Osmos";
}

std::string Osmos::getInstructions()
{
	return "Eat everything that's smaller";
}

void Osmos::loadResources()
{
	background = resourceManager->getResource<blib::Texture>("assets/games/Osmos/back.png");
	ballTexture = resourceManager->getResource<blib::Texture>("assets/games/Osmos/ball.png");
}

void Osmos::start(Difficulty difficulty)
{
	float radius = glm::min(settings->center.x, settings->center.y) - 100;
	for(size_t i = 0; i < players.size(); i++)
	{
		players[i]->sphere.size = 100;
		players[i]->sphere.position = settings->center + radius * blib::util::fromAngle(i/(float)players.size() * 2 * (float)M_PI);
		players[i]->sphere.oldPosition = players[i]->sphere.position;
	}


	float massFillFactor = 0.6f;
	std::for_each(spheres.begin(), spheres.end(), [] (Sphere* s) { delete s; });
	spheres.clear();
	double totalMass = 0;
	while (totalMass < (settings->resX * settings->resY)*massFillFactor)
	{
		glm::vec2 location(blib::math::randomDouble() * settings->resX, blib::math::randomDouble() * settings->resY);
		float maxSize = 25 + (float)blib::math::randomDouble() * 50;
		for (size_t ii = 0; ii < spheres.size() && maxSize > 0; ii++)
		{
			float diff = glm::length(spheres[ii]->position - location);
			diff -= spheres[ii]->size / 2;
			if (diff < maxSize)
				maxSize = diff;
		}
		for (size_t ii = 0; ii < players.size() && maxSize > 0; ii++)
		{
			float diff = glm::length(players[ii]->sphere.position - location);
			diff -= players[ii]->sphere.size / 2;
			if (diff < maxSize)
				maxSize = diff;
		}

		if (maxSize > 0)
		{
			Sphere* s = new Sphere();
			s->position = location;
			s->size = maxSize*2;
			s->oldPosition = location;
			spheres.push_back(s);
			totalMass += s->getMass();
		}
	}

}

void Osmos::update( float elapsedTime )
{
	for(auto player : players)
	{
		glm::vec2 bla = player->sphere.position;
		player->sphere.position += (player->sphere.position - player->sphere.oldPosition) + /*(100000 / (float)glm::pow(player->sphere.size, 1.1f)) * */ elapsedTime * elapsedTime * 1000 * player->joystick.leftStick;

		player->sphere.size -= ((float)glm::pow(glm::length(player->joystick.leftStick), 2.0f * elapsedTime) / 2.0f) * elapsedTime * 0.5f * player->sphere.size;

		player->sphere.oldPosition = bla;

		player->sphere.position.x = glm::min(settings->size.x, glm::max(0.0f, player->sphere.position.x));
		player->sphere.position.y = glm::min(settings->size.y, glm::max(0.0f, player->sphere.position.y));
	}

	for(auto p : players)
	{
		if (p->sphere.size <= 0)
			continue;

		p->testCollision(blib::linq::select<std::vector<Sphere*> >(players, [] (OsmosPlayer* pp) { return &pp->sphere; }));
		p->testCollision(spheres);
	}
}

void Osmos::draw()
{
	spriteBatch->begin();
	spriteBatch->draw(background, glm::mat4());

	for(auto p : players)
		if (p->sphere.size > 0)
			spriteBatch->draw(ballTexture, p->sphere.getMatrix(), ballTexture->center, blib::math::Rectangle(0,0,1,1), p->participant->color);

	for (auto s : spheres)
		spriteBatch->draw(ballTexture, s->getMatrix(), ballTexture->center, blib::math::Rectangle(0,0,1,1), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

	spriteBatch->end();
}

bool Osmos::hasWinner()
{
	return blib::linq::count(players, [] (OsmosPlayer* player) { return player->sphere.size > 0; }) == 1;
}

std::list<Player*> Osmos::getWinners()
{
	return blib::linq::where<std::list<Player*> >(players, [] (OsmosPlayer* player) { return player->sphere.size > 0; });
}

blib::Texture* Osmos::getTitleImage()
{
	return NULL;
}
