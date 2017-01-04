#include "Asteroids.h"
#include "Asteroid.h"

#include "../../playallthegames/Participant.h"
#include "../../playallthegames/Settings.h"

#include <blib/ParticleSystem.h>
#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/Renderer.h>
#include <blib/Math.h>
#include <blib/audio/AudioManager.h>



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
		for (int i = 0; i < 3; i++)
			asteroidTexture[i] = resourceManager->getResource<blib::Texture>("assets/games/Asteroids/asteroid" + std::to_string(i) + ".png");
		bulletTexture = resourceManager->getResource<blib::Texture>("assets/games/Asteroids/bullet.png");

		particleSystem = new blib::ParticleSystem(renderer, resourceManager, spriteBatch);
		particleSystem->resizeGl(settings->resX, settings->resY, 0, 0);
		particleSystem->setTextureFolder("assets/games/Asteroids/particles/");

		laser = audioManager->loadSample("assets/games/Asteroids/laser.wav");
		explosion = audioManager->loadSample("assets/games/Asteroids/explosion.wav");

	}


	void Asteroids::start()
	{
		for (auto p : players)
		{
			p->position = glm::vec2(1920 / 2, 1080 / 2) + 400.0f * blib::math::fromAngle(glm::radians(360.0f / players.size() * p->index));
			p->rotation = 360.0f / players.size() * p->index + 180;
		}
		asteroids.clear();
		bullets.clear();
		particleSystem->clear();
	}
	

	void Asteroids::update(float elapsedTime)
	{
		for (auto p : players)
		{
			if (!p->alive)
				continue;

			p->rotation += p->joystick.leftStick.x * 180.0f * elapsedTime;

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

			if (p->joystick.b == 1 && p->prevJoystick.b != 1)
			{
				bullets.push_back(std::pair<glm::vec4, asteroids::Player*>(glm::vec4(p->position.x, p->position.y, p->movement.x + 300 * cos(glm::radians(p->rotation+180)), p->movement.y + 300 * sin(glm::radians(p->rotation+180))), p));
				laser->play();
			}
		}

		for (Asteroid &a : asteroids)
		{
			a.angle += a.angleIncrement;
			a.position += a.movement * elapsedTime;
			a.fade = glm::min(1.0f, a.fade + elapsedTime);

			while (a.position.x > 1920) // wrap around right side of the screen
				a.position.x -= 1920;
			while (a.position.x < 0) //wrap around left side of the screen
				a.position.x += 1920;
			while (a.position.y > 1080) // wrap around bottom
				a.position.y -= 1080;
			while (a.position.y < 0) //wrap around top
				a.position.y += 1080;
		}

		for (int i = 0; i < (int)bullets.size(); i++)
		{
			auto & b = bullets[i];
			b.first.x += b.first.z * elapsedTime;
			b.first.y += b.first.a * elapsedTime;
		}

		for (size_t ii = 0; ii < asteroids.size(); ii++)
		{
			if (asteroids[ii].fade < 0.75f)
				continue;



			bool hit = false;
			for (int i = 0; i < (int)bullets.size(); i++)
			{
				auto & b = bullets[i];
				float dist = glm::distance(glm::vec2(b.first.x, b.first.y), asteroids[ii].position);
				if (asteroids[ii].size == 0 && dist < 16)
					hit = true;
				if ((asteroids[ii].size == 1 && dist < 32) || (asteroids[ii].size == 2 && dist < 64))
					hit = true;
				if(hit)
					bullets.erase(bullets.begin() + i);
			}

			for (auto p : players)
			{
				float dist = glm::distance(p->position, asteroids[ii].position);
				if (dist < 20+ (16<<asteroids[ii].size))
				{
					hit = true;
					p->alive = false;

					blib::Emitter* emitter = particleSystem->addEmitter("assets/games/Asteroids/particles/explosion.json");
					emitter->position = emitter->prevPosition = p->position;
					emitter->life = 0.5f;

				}
			}

			if(hit)
			{
				explosion->play();
				int newSize = asteroids[ii].size - 1;
				if (asteroids[ii].size > 0)
				{
					int spawnCount = 6;
					if (difficulty == Difficulty::Easy)
						spawnCount = 4;
					else if (difficulty == Difficulty::Hard)
						spawnCount = 7;
					else if (difficulty == Difficulty::Cruel)
						spawnCount = 10;
					else if (difficulty == Difficulty::Wtf)
						spawnCount = 50;

					for (int iii = 0; iii < 6; iii++)
					{

						float speed = 4;
						if (difficulty == Difficulty::Hard || difficulty == Difficulty::Cruel)
							speed = 3;


						if (difficulty == Difficulty::Wtf)
						{
							speed = blib::math::randomFloat(0, 4);
							if (rand() % 100 < 5)
								speed = 10;
						}


						Asteroid a;
						a.movement = asteroids[ii].movement + speed * glm::length(asteroids[ii].movement) * blib::math::fromAngle(blib::math::randomFloat(0, 2 * blib::math::pif));
						a.size = newSize;
						a.position = asteroids[ii].position;
						asteroids.push_back(a); //add an asteroid
					}
				}
				asteroids.erase(asteroids.begin() + ii);
				break;
			}
		}

		for (int i = 0; i < (int)bullets.size(); i++)
		{
			auto & b = bullets[i];
			b.first.x += b.first.z * elapsedTime;
			b.first.y += b.first.a * elapsedTime;
		}


		int asteroidCount = 25;
		if (difficulty == Difficulty::Easy)
			asteroidCount = 15;
		else if (difficulty == Difficulty::Hard)
			asteroidCount = 40;
		else if (difficulty == Difficulty::Cruel)
			asteroidCount = 100;
		else if (difficulty == Difficulty::Wtf)
			asteroidCount = 500;


		if (asteroids.size() < asteroidCount)
		{
			Asteroid a;
			bool blocks = blib::linq::any(players, [&a](asteroids::Player* p) { return glm::distance(a.position, p->position) < 128; });
			
			if(!blocks)
				asteroids.push_back(a); //add an asteroid
		}
		particleSystem->update(elapsedTime);



	}

	void Asteroids::draw()
	{
		spriteBatch->begin();

		spriteBatch->draw(backgroundTexture, blib::math::easyMatrix(glm::vec2(0, 0)));

		for(auto p : players)
			if(p->alive)
				spriteBatch->draw(playerTexture, blib::math::easyMatrix(p->position, p->rotation), playerTexture->center, p->participant->color);
		
		for (const Asteroid &asteroid : asteroids)
			spriteBatch->draw(asteroidTexture[asteroid.size], blib::math::easyMatrix(asteroid.position, asteroid.angle), asteroidTexture[asteroid.size]->center, glm::vec4(1,1,1,asteroid.fade));


		for (const auto & b : bullets)
		{
			spriteBatch->draw(bulletTexture, blib::math::easyMatrix(glm::vec2(b.first.x, b.first.y), glm::degrees(atan2(b.first.a, b.first.z))), bulletTexture->center, b.second->participant->color);
		}


		spriteBatch->end();
		particleSystem->renderState.activeFbo = spriteBatch->renderState.activeFbo;
		particleSystem->draw(glm::mat4());

	}




	blib::Texture* Asteroids::getTitleImage()
	{
		return NULL;
	}

}