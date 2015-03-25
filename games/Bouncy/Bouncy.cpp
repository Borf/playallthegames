#include "Bouncy.h"

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

#include <blib/Renderer.h>
#include <blib/RenderState.h>
#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/util/Profiler.h>
#include <blib/Color.h>
#include <blib/Math.h>
#include <blib/LineBatch.h>


Bouncy::Bouncy()
{
	backgroundColor = glm::vec4(0,0,0,1);
}

std::string Bouncy::getName()
{
	return "Bouncy";
}

std::string Bouncy::getInstructions()
{
	return "Dodge the objects";
}


void Bouncy::loadResources()
{
	playerSprite = resourceManager->getResource<blib::Texture>("assets/games/Bouncy/player.png");
	objectsSprite = resourceManager->getResource<blib::Texture>("assets/games/Bouncy/shapes.png");
}

void Bouncy::start(Difficulty difficulty)
{

	objects.clear();
}

void Bouncy::update( float elapsedTime )
{
	if (blib::math::randomDouble() < 0.015 * elapsedTime * 60.0f)
	{
		int dir = rand() % 2;

		int h = (int)(objectsSprite->height * 1.4142135623730950488016887242097);

		BouncyObject o;
		o.pos = glm::vec2(dir * 1920, h / 2 + 540-300 + h * (rand() % ((600 - objectsSprite->height) / h))),
		o.speed =(-1+dir*2) * -8.0f,
		o.sprite = rand()%4;
		objects.push_back(o);
	}

	for (auto p : players)
	{
		if(!p->alive)
			continue;
		p->position += p->speed * elapsedTime * 60.0f;

		p->speed.x *= glm::pow(0.9f, elapsedTime*60);
		p->speed.x += 0.9f * p->joystick.leftStick.x;

		p->speed.x = glm::min(8.0f, glm::max(-8.0f, p->speed.x));

		if ((p->position.y < 540 - 300 && p->speed.y < 0) || (p->position.y > 540 + 300 - playerSprite->height && p->speed.y > 0))
			p->speed.y = -p->speed.y;


		blib::math::Rectangle playerRect(p->position, (float)playerSprite->width, (float)playerSprite->height);
		playerRect.inflate(-6, -6);

		for (auto o : objects)
		{
			blib::math::Rectangle objRect(o.pos - glm::vec2(objectsSprite->width/8.0f, objectsSprite->height/2.0f), objectsSprite->width/4.0f, (float)objectsSprite->height);
			objRect.inflate(-6, -6);
			if(objRect.intersect(playerRect))
				p->alive = false;
		}
	}


	for (int i = objects.size() - 1; i >= 0; i--)
	{
		objects[i].pos.x += objects[i].speed * elapsedTime * 60;
		if (objects[i].pos.x < 0 || objects[i].pos.x > 1920)
			objects.erase(objects.begin() + i);
	}
}

void Bouncy::draw()
{
	spriteBatch->begin(settings->scaleMatrix);

	int frame = (int)(blib::util::Profiler::getAppTime() / 9);

	/*for (int i = -1; i < 2; i++)
	{
		SpriteBatch.Draw(lineSheet, 0, frame, new Vector2(0, SystemSettings.ResolutionY / 2-300 + i), Color.White);
		SpriteBatch.Draw(lineSheet, 0, frame, new Vector2(0, SystemSettings.ResolutionY / 2+300 + i), Color.White);
	}*/


	int height = 10;
	lineBatch->begin();
	for (int x = 0; x < settings->resX; x++)
	{
		float y = (float)(height * glm::cos((x*0.01 + blib::util::Profiler::getAppTime()*10) + (height/2)*glm::sin(x / 150.0) * glm::cos(x / 100.0)));
		lineBatch->draw(glm::vec2(x, y-2+settings->center.y + 300*settings->scale), glm::vec2(x, y+2+settings->center.y+300*settings->scale), glm::vec4(0,1,0,1));
		lineBatch->draw(glm::vec2(x, y-2+settings->center.y - 300*settings->scale), glm::vec2(x, y+2+settings->center.y-300*settings->scale), glm::vec4(0,1,0,1));
	}
	lineBatch->end();



	for (auto p : players)
		if(p->alive)
			spriteBatch->draw(playerSprite, p->getMatrix(), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), p->participant->color);

	for (auto o : objects)
		spriteBatch->draw(objectsSprite, o.getMatrix(), objectsSprite->center / glm::vec2(4,1), blib::math::Rectangle(0.25f * o.sprite,0.0f,0.25f,1.0f), blib::Color::green);

	spriteBatch->end();

}

blib::Texture* Bouncy::getTitleImage()
{
	return NULL;
}
