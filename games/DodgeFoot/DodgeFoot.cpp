#include "DodgeFoot.h"

#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/ResourceManager.h>
#include <blib/Math.h>
#include <blib/Util.h>

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

std::string DodgeFoot::getName()
{
	return "DodgeFoot";
}

std::string DodgeFoot::getInstructions()
{
	return "Dodge the foot";
}

void DodgeFoot::loadResources()
{
	playerSprite =	resourceManager->getResource<blib::Texture>("assets/games/dodgefoot/player.png");
	footSprite =	resourceManager->getResource<blib::Texture>("assets/games/dodgefoot/foot.png");
	floorSprite =	resourceManager->getResource<blib::Texture>("assets/games/dodgefoot/grass.png");
	skySprite =		resourceManager->getResource<blib::Texture>("assets/games/dodgefoot/sky.png");
	redFootSprite = resourceManager->getResource<blib::Texture>("assets/games/dodgefoot/redfoot.png");
	footMaskSprite =resourceManager->getResource<blib::Texture>("assets/games/dodgefoot/footmask.png");

}

void DodgeFoot::start(Difficulty difficulty)
{
	for(auto p : players) { p->position = (p->index+1) * (1920 / (players.size()+1.0f)); }
	footPosition = glm::vec2(1000, 0);
	targetFootLocation = blib::math::randomFloat(1920);
	footMovingDown = false;
	lastHit = 0;
	speed = 1;
}

void DodgeFoot::update( float elapsedTime )
{
	speed += 0.002f * elapsedTime * 60;
	if (footPosition.y <= 0)
	{
		footPosition.y = 0;

		if (glm::abs(targetFootLocation - footPosition.x) < 10 * speed)
		{
			footMovingDown = true;
			footPosition.y += 1 * elapsedTime * 60;
		}

		if (footPosition.x < targetFootLocation)
			footPosition.x += 10 * speed * elapsedTime * 60;
		else
			footPosition.x -= 10 * speed * elapsedTime * 60;
	}
	else
	{
		if (footMovingDown)
			footPosition.y += 25.0f * speed * elapsedTime * 60;
		else
		{
			footPosition.y -= 25.0f * speed * elapsedTime * 60;
			if (footPosition.y <= 0)
				targetFootLocation = blib::math::randomFloat(1920);
		}

		if (footPosition.y > 700 && footMovingDown)
		{
			footMovingDown = !footMovingDown;
			screenShake = 0.25f;
		}
	}


	blib::math::Rectangle boundsFoot(footPosition, footSprite->originalWidth, footSprite->originalHeight);
	for(auto p : players)
	{
		if (!p->alive)
			continue;
		blib::math::Rectangle boundsPlayer = p->getBounds(playerSprite);
		if (boundsPlayer.intersect(boundsFoot))
		{
		//	particleEmitters.Add(new BloodEmitter(new Vector2(p.getBounds(playerSprite).Center.x, p.getBounds(playerSprite).Center.y), elapsedTime) { floor = 1060, range = 20 });
			p->alive = false;
			lastHit = 1;
		}

		p->position += speed * 20 * p->joystick.leftStick.x * elapsedTime * 60;
		if (p->position < 0)
			p->position = 0;
		if (p->position > 1920 - playerSprite->originalWidth)
			p->position = 1920.0f - playerSprite->originalWidth;

		for(auto pp : players)
		{
			if (p == pp || !p->alive || !pp->alive)
				continue;
			float diff = p->position - pp->position;
			if (glm::abs(diff) < playerSprite->originalWidth)
			{
				if (diff < 0)
					diff = (diff + playerSprite->originalWidth) / 2;
				else
					diff = (diff - playerSprite->originalWidth) / 2;
				p->position -= diff;
				pp->position += diff;

			}
		}
	}

	lastHit -= 0.01f * elapsedTime * 60;
	if (lastHit < 0)
		lastHit = 0;
	screenShake -= elapsedTime;
	if (screenShake < 0)
		screenShake = 0;
}

void DodgeFoot::draw()
{
	glm::vec2 screenOffset(0,0);
	if (screenShake > 0)
	{
		screenOffset = glm::vec2(5 * glm::sin(screenShake * 50 * speed), 5 * glm::cos(screenShake * 60 * speed));
	}


	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(skySprite, blib::math::easyMatrix(screenOffset));
	spriteBatch->draw(floorSprite, blib::math::easyMatrix(glm::vec2(0, 1040) + screenOffset));

	for(auto p : players)
	{
		if(p->alive)
			spriteBatch->draw(playerSprite, blib::math::easyMatrix(glm::vec2(p->position, 1000) + screenOffset), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), p->participant->color);
	}

	spriteBatch->draw(footSprite, blib::math::easyMatrix(footPosition + screenOffset));
	spriteBatch->draw(redFootSprite, blib::math::easyMatrix(footPosition + screenOffset), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), glm::vec4(lastHit, lastHit, lastHit, lastHit));
	spriteBatch->end();

}

blib::Texture* DodgeFoot::getTitleImage()
{
	return NULL;
}
