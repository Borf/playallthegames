#include "BulletHell.h"

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Renderer.h>
#include <blib/VBO.h>
#include <blib/Texture.h>
#include <blib/Math.h>
#include <blib/Color.h>
#include <blib/Util.h>
#include <blib/math/Line.h>

#include "RotatingBulletGenerator.h"
#include "Bullet.h"


BulletHell::BulletHell( )
{

}


std::string BulletHell::getName()
{
	return "BulletHell";
}

std::string BulletHell::getInstructions()
{
	return "Dodge the astroids";
}

void BulletHell::loadResources()
{
	background =			resourceManager->getResource<blib::Texture>("assets/games/BulletHell/back.png");
	shipSprite =			resourceManager->getResource<blib::Texture>("assets/games/BulletHell/ship.png");
	shipOverlaySprite =		resourceManager->getResource<blib::Texture>("assets/games/BulletHell/shipOverlay.png");
	bulletSprite =			resourceManager->getResource<blib::Texture>("assets/games/BulletHell/bullet.png");
//	backSprite = new DrawableTexture(Graphics.GraphicsDevice, SystemSettings.ResolutionX, SystemSettings.ResolutionY);// gameContent.Load<Texture2D>("bullethell/back");
}

void BulletHell::start(Difficulty difficulty)
{
	bulletSpawnDelay = 0.6f;
	blib::linq::foreach(players, [this] (BulletHellPlayer* p) { p->position = glm::vec2(300, (p->index+1) * (settings->resY / (players.size()+1))); } );

	blib::linq::deleteall(bullets);
	blib::linq::deleteall(bulletGenerators);

	bulletGenerators.push_back(new RotatingBulletGenerator(glm::vec2(960, 10), bulletSpawnDelay));
	bulletGenerators.push_back(new RotatingBulletGenerator(glm::vec2(960, settings->resY-10), bulletSpawnDelay));
	totalGameTime = 0;
	speed = 1;
}


void BulletHell::update( float elapsedTime )
{
	totalGameTime += elapsedTime;
	speed += 0.1f * elapsedTime;

	for(auto player : players)
	{
		player->rotation += 60*elapsedTime * 5 * player->joystick.rightTrigger;
		player->rotation -= 60*elapsedTime * 5 * player->joystick.leftTrigger;
		player->position += elapsedTime * settings->scale * 500 * player->joystick.leftStick;

		player->position.x = glm::max(0.0f, glm::min((float)settings->resX, player->position.x));
		player->position.y = glm::max(0.0f, glm::min((float)settings->resY, player->position.y));
	}


	blib::linq::foreach(bullets, [this, elapsedTime] (Bullet* b) { b->update(elapsedTime, settings); });

	for (auto g : bulletGenerators)
	{
		Bullet* newBullet = g->getBullet(totalGameTime, speed);
		if (newBullet != NULL)
			bullets.push_back(newBullet);
	}


	for(auto player : players)
	{
		if (!player->alive)
			continue;
		glm::vec2 p1 = player->position + 70.0f * blib::util::fromAngle(player->rotation);
		glm::vec2 p2 = player->position - 70.0f * blib::util::fromAngle(player->rotation);
		blib::math::Line playerLine(p1, p2);

		for(auto b : bullets)
		{
			glm::vec2 closest = playerLine.project(b->pos);
			if (glm::length(closest - b->pos) < (bulletSprite->width / 2 + 15))
				player->alive = false;
		}

	}
}

void BulletHell::draw()
{
	spriteBatch->begin();
	spriteBatch->draw(background, settings->scaleMatrix);
	for(auto player : players)
	{
		if (player->alive)
		{
			spriteBatch->draw(shipSprite, player->getMatrix(), shipSprite->center, blib::math::Rectangle(0,0,1,1), player->participant->color);
			spriteBatch->draw(shipOverlaySprite, player->getMatrix(), shipSprite->center, blib::math::Rectangle(0,0,1,1), blib::Color::white);
		}
	}

	for(auto b : bullets)
		spriteBatch->draw(bulletSprite, b->getMatrix(), bulletSprite->center, blib::math::Rectangle(0,0,1,1), b->color);

	spriteBatch->end();
}

blib::Texture* BulletHell::getTitleImage()
{
	return NULL;
}
