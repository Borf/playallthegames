#include "ZombieSurvival.h"

#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>

#include "../../Settings.h"
#include "../../Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>

GameBase* getGame()
{
	return new ZombieSurvival();
}

std::string ZombieSurvival::getName()
{
	return "ZombieSurvival";
}

std::string ZombieSurvival::getInstructions()
{
	return "Stay Alive";
}

void ZombieSurvival::loadResources()
{
	backSprite = resourceManager->getResource<blib::Texture>("assets/games/ZombieSurvival/back.png");
}

void ZombieSurvival::start(Difficulty difficulty)
{


}

void ZombieSurvival::update(float elapsedTime)
{
}

void ZombieSurvival::draw()
{
	spriteBatch->begin();
	spriteBatch->draw(backSprite, glm::mat4());


	spriteBatch->end();
}

blib::Texture* ZombieSurvival::getTitleImage()
{
	return NULL;
}
