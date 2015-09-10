#include "Asteroids.h"

#include "../../playallthegames/Participant.h"

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/Renderer.h>
#include <blib/Math.h>



namespace asteroids
{
	std::string Asteroids::getName()
	{
		return "Asteroids";
	}
	std::string Asteroids::getInstructions()
	{
		return "Blast asteroids and survive";
	}
	
	void Asteroids::loadResources()
	{
		backgroundTexture = resourceManager->getResource<blib::Texture>("assets/games/Asteroids/back.png");
		playerTexture = resourceManager->getResource<blib::Texture>("assets/games/Asteroids/player.png");
	}


	void Asteroids::start(Difficulty difficulty)
	{
		for (auto p : players)
			p->position = glm::vec2(1920 / 2, 1080 / 2) + 400.0f * blib::math::fromAngle(glm::radians(360.0f / players.size() * p->index));

	}
	

	void Asteroids::update(float elapsedTime)
	{
		for (auto p : players)
		{
			if (!p->alive)
				continue;

			p->rotation += p->joystick.leftStick.x * 90.0f * elapsedTime;

			if(p->joystick.a == 1)
				p->movement += blib::math::fromAngle(glm::radians(p->rotation+180)) * elapsedTime * 500.0f;
			p->position += p->movement * elapsedTime;

			while (p->position.x > 1920) // wrap around right side of the screen
				p->position.x -= 1920;
			while (p->position.x < 0) //wrap around left side of the screen
				p->position.x += 1920; 
			while (p->position.y > 1080) // wrap around bottom
				p->position.y -= 1080;
			while (p->position.y < 0) //wrap around top
				p->position.y += 1080;
		}

		

	}

	void Asteroids::draw()
	{
		spriteBatch->begin();

		spriteBatch->draw(backgroundTexture, blib::math::easyMatrix(glm::vec2(0, 0)));

		for(auto p : players)
			spriteBatch->draw(playerTexture, blib::math::easyMatrix(p->position, p->rotation), playerTexture->center, p->participant->color);


		spriteBatch->end();

	}




	blib::Texture* Asteroids::getTitleImage()
	{
		return NULL;
	}

}