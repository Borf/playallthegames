#include "FlyFar.h"

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/Util.h>
#include <blib/util/Log.h>
#include <blib/Box2DDebug.h>
#include <blib/Math.h>
#include <blib/Renderer.h>

#include <Box2D/Box2D.h>
#include <glm/gtc/matrix_transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>


using blib::util::Log;


FlyFar::FlyFar()
{
	backgroundColor = glm::vec4(73.0f/255, 96.0f/255, 172.0f/255,1);
}

std::string FlyFar::getName()
{
	return "FlyFar";
}

std::string FlyFar::getInstructions()
{
	return "Fly as far as possible";
}

void FlyFar::loadResources()
{
	flyerSprite = resourceManager->getResource<blib::Texture>("assets/games/FlyFar/flyer.png");
	rampSprite = resourceManager->getResource<blib::Texture>("assets/games/FlyFar/ramp.png");
	housesSprite = resourceManager->getResource<blib::Texture>("assets/games/FlyFar/houses.png");
	gustSprite = resourceManager->getResource<blib::Texture>("assets/games/FlyFar/gust.png");
	balloonSprite = resourceManager->getResource<blib::Texture>("assets/games/FlyFar/balloon.png");
}

void FlyFar::start(Difficulty difficulty)
{
	for(size_t i = 0; i < players.size(); i++)
	{
		players[i]->position = glm::vec2(0, 1080 - rampSprite->height-50);
	}
	lastScale = 1;
	lastLeft = 0;
	gusts.clear();
	balloons.clear();


	gustFactor = 2;
}

void FlyFar::update( float elapsedTime )
{
	float left = players[0]->position.x;
	float right = players[0]->position.x;
	for(size_t i = 0; i < players.size(); i++)
	{
		if(players[i]->position.y <= 1080)
		{
			left = glm::min(left, players[i]->position.x);
			right = glm::max(right, players[i]->position.x);
		}
	}

	float scale = (1920-600) / (float)(right - left);
	if (scale > 1)
		scale = 1;

	lastScale = 0.9f * lastScale + 0.1f * scale;
	lastLeft = 0.9f * lastLeft + 0.1f * left;

	float factor = elapsedTime*60;

	if (blib::math::randomDouble() < gustFactor*pow((left / 10 + 250), -0.7)*factor)
		gusts.push_back(glm::vec2(glm::max(left+1920, right) + 600 / lastScale, 400+(float)blib::math::randomDouble() * 300));

	for (auto p : players)
	{
		if (p->position.x < 1050)
		{
			p->rotation = (float)M_PI / 4;
			p->position += factor * p->speed * blib::util::fromAngle(p->rotation);
			p->speed += factor * .5f;
		}
		else if(p->position.x < 1350)
		{
			p->rotation = (float)M_PI / -4;
			p->position += factor * p->speed * blib::util::fromAngle(p->rotation);
		}
		else
		{
			if (p->position.y <= 1080)
			{
			/*	p->position += factor * p->speed * blib::util::fromAngle(p->rotation);
				p->position.y += glm::max(0.0f, factor * (45 - p->speed)) * .05f; //gravity

				p->rotation += (.003f)*glm::max(0.0f, factor * (10 - p->speed*0.1f));

				p->rotation += factor*0.06f*p->joystick.leftStick.x;

				if (p->rotation > M_PI/2)
					p->rotation = (float) M_PI/2;
				if (p->rotation < -M_PI/2)
					p->rotation = (float) -M_PI/2;

				p->speed += factor * (p->rotation);
				p->speed *= pow(0.995f, 30*elapsedTime);
				if (p->speed < 0.1f)
					p->speed = 0.1f;*/
				p->position += factor * p->speed * blib::util::fromAngle(p->rotation);
				p->position.y += glm::max(0.0f, factor * (45 - p->speed)) * .01f; //gravity

				p->rotation += (.003f)*glm::max(0.0f, factor * (5 - p->speed*0.5f));

				p->rotation += factor*0.06f*p->joystick.leftStick.x;

				if (p->rotation > M_PI / 2)
					p->rotation = (float)M_PI / 2;
				if (p->rotation < -M_PI / 2)
					p->rotation = (float)-M_PI / 2;

				p->speed += factor * (p->rotation);
				p->speed *= (float)pow(0.995, 15 * elapsedTime);
				if (p->speed < 0.1f)
					p->speed = 0.1f;
				for (auto g : gusts)
				{
					if (glm::length(p->position - (g + gustSprite->center)) < 100)
						p->moveUp = 100;
				}

				if (p->moveUp > 0)
				{
					p->position.y -= 5*factor;
					p->moveUp -= 5*factor;
				}

			}
		}
	}


}

void FlyFar::draw()
{
	glm::mat4 transform = settings->scaleMatrix;
	transform = glm::translate(transform, glm::vec3(0, 1080 - 1080 * lastScale, 0));
	transform = glm::scale(transform, glm::vec3(lastScale, lastScale, 1));
	transform = glm::translate(transform, glm::vec3(-lastLeft, 0,0));
	spriteBatch->begin(transform);

	for (int i = 480 * ((int)(lastLeft-200/lastScale) / 480); i <= 480 * ((int)lastLeft / 480) + (1920+480) / lastScale; i += 480)
		spriteBatch->draw(housesSprite, glm::translate(glm::mat4(), glm::vec3(i, 1080 - rampSprite->height,0)));

	spriteBatch->draw(rampSprite, glm::translate(glm::mat4(), glm::vec3(0, 1080 - rampSprite->height,0)));

	for(auto player : players)
		spriteBatch->draw(flyerSprite, glm::rotate(glm::translate(glm::mat4(), glm::vec3(player->position,0)), glm::degrees(player->rotation), glm::vec3(0,0,1)), flyerSprite->center, blib::math::Rectangle(0,0,1,1), player->participant->color);

	for(auto g : gusts)
		spriteBatch->draw(gustSprite, glm::translate(glm::mat4(), glm::vec3(g,0)));

/*	foreach (Vector2 balloon in balloons)
		SpriteBatch.Draw(balloonSprite, balloon, null, Color.White);

	SpriteBatch.DrawString(Font, Math.Round(players.Min(p => p.position.X) / 100, 1) + " - " + Math.Round(players.Max(p => p.position.X) / 100, 1) + " meter", new Vector2(lastLeft, SystemSettings.ResolutionY - SystemSettings.ResolutionY / lastScale), Color.White, 0, Vector2.Zero, 1 / lastScale, SpriteEffects.None, 0);

	SpriteBatch.End();
*/

	spriteBatch->end();

}

blib::Texture* FlyFar::getTitleImage()
{
	return NULL;
}

bool FlyFar::hasWinner()
{
	int deadCount = 0;
	float furthest = 0;
	float furthestAlive = 0;
	for(auto p : players)
	{
		if (p->position.y > 1080)
			deadCount++;
		else
			furthestAlive = glm::max(p->position.x, furthest);
		furthest = glm::max(p->position.x, furthest);
	}

	if(deadCount == players.size())
		return true;
	if(deadCount == players.size()-1 && furthestAlive == furthest)
		return true;
	return false;
}

std::list<Player*> FlyFar::getWinners()
{
	std::list<Player*> ret;
	int deadCount = 0;
	float furthest = 0;
	for(auto p : players)
		furthest = glm::max(p->position.x, furthest);
	for(auto p : players)
		if(p->position.x == furthest)
			ret.push_back(p);
	return ret;
}
