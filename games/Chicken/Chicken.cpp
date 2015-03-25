#include "Chicken.h"

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

#include <blib/Texture.h>
#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/linq.h>

#include <glm/gtc/matrix_transform.hpp>

Chicken::Chicken()
{

}

std::string Chicken::getName()
{
	return "Chicken";
}

std::string Chicken::getInstructions()
{
	return "Don't fall off at the end of the cliff";
}

void Chicken::loadResources()
{
	backSprite =		resourceManager->getResource<blib::Texture>("assets/games/Chicken/back.png");
	finalBackSprite =	resourceManager->getResource<blib::Texture>("assets/games/Chicken/finalBack.png");
	carSprite =			resourceManager->getResource<blib::Texture>("assets/games/Chicken/car.png");
	carOverlaySprite =	resourceManager->getResource<blib::Texture>("assets/games/Chicken/carOverlay.png");
}

void Chicken::start(Difficulty difficulty)
{
	scroll = 0;
	distance = 3000;

	for(auto p : players)
	{
		p->position = glm::vec2(3000,0);
		p->driving = true;
	}

}

void Chicken::update( float elapsedTime )
{
	if (distance > 0)
	{
		scroll -= 10*elapsedTime*60;
		while (scroll > 1920)
			scroll -= 1920;
		while (scroll < 0)
			scroll += 1920;

		distance -= 10*elapsedTime*60;
	}

	for(auto p : players)
	{
		if(!p->driving)
			continue;
		p->position.x -= 11*elapsedTime*60;
		if (p->position.x < 0)
			p->position.y += 20*elapsedTime*60;
	}

	for(auto p : players)
	{
		if(p->position.x <= 0)
			continue;
		if (p->joystick.a == 1)
			p->driving = false;
	}

}

void Chicken::draw()
{
	spriteBatch->begin(settings->scaleMatrix);

	spriteBatch->draw(backSprite, glm::translate(glm::mat4(), glm::vec3(scroll - 1920, 0,0)));
	spriteBatch->draw(backSprite, glm::translate(glm::mat4(), glm::vec3(scroll, 0,0)));
	spriteBatch->draw(backSprite, glm::translate(glm::mat4(), glm::vec3(scroll + 1920, 0,0)));

	spriteBatch->draw(finalBackSprite, glm::translate(glm::mat4(), glm::vec3(distance, 0,0)));


	float spacing = (1080 - 150) / (float)(players.size()+1);
	for(auto p : players)
	{
		spriteBatch->draw(carSprite, glm::translate(glm::mat4(), glm::vec3(distance - p->position.x, 150 + p->index * spacing + p->position.y,0)));
		spriteBatch->draw(carOverlaySprite, glm::translate(glm::mat4(), glm::vec3(distance - p->position.x, 150 + p->index * spacing + p->position.y,0)), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), p->participant->color);
	}

	spriteBatch->end();
}

blib::Texture* Chicken::getTitleImage()
{
	return NULL;
}

bool Chicken::hasWinner()
{
	return blib::linq::count(players, [] (ChickenPlayer* p) { return p->position.x < -500 || !p->driving; }) == players.size();
}

std::list<Player*> Chicken::getWinners()
{
	std::vector<ChickenPlayer*> alivePlayers = blib::linq::where(players, [] (ChickenPlayer* p) { return p->position.x >= 0; });
	float minValue = blib::linq::min<float>(alivePlayers, [] (ChickenPlayer* p) { return p->position.x; });
	return blib::linq::where<std::list<Player*> >(alivePlayers, [minValue] (ChickenPlayer* p) { return p->position.x == minValue; });
}
