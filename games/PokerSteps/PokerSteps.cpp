#include "PokerSteps.h"

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/Math.h>
#include <blib/Color.h>
#include <blib/Util.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

PokerSteps::PokerSteps()
{
	maxPlayerScore = 15;
}


std::string PokerSteps::getName()
{
	return "PokerSteps";
}

std::string PokerSteps::getInstructions()
{
	return "Press a button to vote for a number; only unique votes count!";
}

void PokerSteps::loadResources()
{
	backSprite =	resourceManager->getResource<blib::Texture>("assets/games/PokerSteps/back.png");
	playerSprite =	resourceManager->getResource<blib::Texture>("assets/games/PokerSteps/player.png");
	buttonsSprite =	resourceManager->getResource<blib::Texture>("assets/games/PokerSteps/buttons.png");
	font =			resourceManager->getResource<blib::Font>("lindsey");
}

void PokerSteps::start(Difficulty difficulty)
{
	stateStart = 0;
	state = VOTING;
}

void PokerSteps::update( float elapsedTime )
{
	stateStart += elapsedTime;
	if (state == VOTING)
	{
		for (auto p : players)
		{
			if (p->joystick.x == 1)
				p->choice = 1;
			if (p->joystick.y == 1)
				p->choice = 3;
			if (p->joystick.b == 1)
				p->choice = 5;
		}
		if (stateStart > 5 || blib::linq::count(players, [] (PokerStepsPlayer* p) { return p->choice == 0; }) == 0)
		{
			state = SHOWING;
			stateStart = 0;
		}
	}
	if (state == SHOWING)
	{
		if (stateStart > 2)
		{
			state = WALKING;
			stateStart = 0;
		}
	}
	if (state == WALKING)
	{
		if (blib::linq::count(players, [] (PokerStepsPlayer* p) { return p->choice > 0; }) > 0)
		{
			for(auto p : players)
			{
				if (blib::linq::count(players, [p](PokerStepsPlayer* pp) { return p->choice == pp->choice; })  == 1)
					p->score = glm::min(15, p->score + p->choice);
			}
			for (auto p : players)
				p->choice = 0;
		}

		float fac = glm::pow(0.98f, elapsedTime*60);

		for (auto p : players)
		{
			if (p->posLeft < (20 + p->score * 120))
				p->posLeft = fac * p->posLeft + (1-fac) * (30 + p->score * 120);

		}
		if (blib::linq::count(players, [](PokerStepsPlayer* p) { return (((20 + p->score * 120) - p->posLeft) < 10); }) == players.size())
		{
			state = VOTING;
			if(hasWinner())
			{
				state = WON;
				return;
			}

			stateStart = 0;
		}
	}
}

void PokerSteps::draw()
{
	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(backSprite, glm::mat4());
	for (auto p : players)
		spriteBatch->draw(playerSprite, blib::math::easyMatrix(glm::vec2(p->posLeft, 200 + p->index * 200)), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), p->participant->color);


	if (state == VOTING)
	{
		spriteBatch->draw(font, "Please vote with your controller", blib::math::easyMatrix(glm::vec2(600,120)));
		spriteBatch->draw(buttonsSprite, blib::math::easyMatrix(glm::vec2(950, 300)));
		spriteBatch->draw(font, blib::util::toString(blib::math::round(5 - stateStart,2)), blib::math::easyMatrix(glm::vec2(600, 520)));
	}

	if (state == SHOWING)
	{
		for(auto p : players)
			spriteBatch->draw(font, blib::util::toString(p->choice), blib::math::easyMatrix(glm::vec2(600, 200 + p->index * 200)));
	}


	spriteBatch->end();
}

blib::Texture* PokerSteps::getTitleImage()
{
	return NULL;
}

