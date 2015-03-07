#include "ZombieSurvival.h"

#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>
#include <blib/Animation.h>
#include <blib/math/Polygon.h>
#include <blib/math/Line.h>
#include <blib/Renderer.h>
#include <blib/RenderState.h>
#include <blib/VBO.h>
#include <blib/FBO.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

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

	visionFbo = resourceManager->getResource<blib::FBO>();
	visionFbo->setSize(settings->resX, settings->resY);
	visionFbo->stencil = true;
	visionFbo->depth = false;
	visionFbo->textureCount = 1;

}

void ZombieSurvival::start(Difficulty difficulty)
{
	for (auto p : players)
	{
		p->position = glm::vec2(1920 / 2, 1080 / 2) + 150.0f * blib::util::fromAngle(p->index / (float)players.size() * 2 * blib::math::pif);
		p->rotation = p->index / (float)players.size() * 360.0f;
		p->playerAnimation = new blib::Animation("assets/games/ZombieSurvival/player.png.json", resourceManager);
		p->playerAnimation->setState("idle");

	}


	objects.clear();

	objects.push_back(blib::math::Polygon({ glm::vec2(0, 0), glm::vec2(1920, 0), glm::vec2(1920, 1080), glm::vec2(0, 1080) }));


	for (int x = 0; x < 10; x++)
		for (int y = 0; y < 6; y++)
		{
			glm::vec2 pos(200 * x+50, 200 * y+50);
			objects.push_back(blib::math::Polygon({ pos + glm::vec2(0, 0), pos + glm::vec2(0, 100), pos + glm::vec2(100, 100), pos + glm::vec2(100, 0) }));
		}

	blib::linq::deleteall(zombies);
	for (int i = 0; i < 50; i++)
	{
		spawnZombie();
	}

}


void ZombieSurvival::spawnZombie()
{
	Zombie* z = new Zombie();
	z->zombieSprite = new blib::Animation("assets/games/ZombieSurvival/zombie.json", resourceManager);

	bool ok = false;
	while (!ok)
	{
		z->position = glm::vec2(rand() % 1920, rand() % 1080);
		for (auto p : players)
		{
			blib::math::Line ray(p->position, z->position);
			ok = false;
			for (auto o : objects)
				if (o.intersects(ray))
				{
					ok = true;
					break;
				}
			if (!ok)
				break;
		}
	}

	z->target = players[rand() % players.size()];

	zombies.push_back(z);
}



void ZombieSurvival::update(float elapsedTime)
{
	static float spawnTimer = 0;
	spawnTimer += elapsedTime;
	while (spawnTimer > 0.25f)
	{
		//if (rand() % 50 == 0)
			spawnZombie();
		spawnTimer -= 0.25f;
	}

	for (auto p : players)
	{
		p->playerAnimation->update(elapsedTime);
		if (!p->alive)
			continue;
		glm::vec2 oldPos = p->position;
		p->position += p->joystick.leftStick * elapsedTime * 200.0f;
		blib::math::Line ray(p->position, oldPos);
		glm::vec2 point;
		blib::math::Line hitLine;
		for (auto o : objects)
			if (o.intersects(ray, point, hitLine))
			{
				float overShoot = glm::length(p->position - point);
				p->position += overShoot * -hitLine.normal();
				break;
			}


		if (glm::length(p->joystick.leftStick) > 0.1f)
		{
			p->rotation = glm::degrees(atan2(p->joystick.leftStick.y, p->joystick.leftStick.x));
			p->accuracy = glm::max(1.0f, p->accuracy);
			p->accuracy /= glm::pow(0.01f, elapsedTime);
			p->accuracy = glm::min(30.0f, p->accuracy);
			p->playerAnimation->setState("walk");
		}
		else
		{
			p->playerAnimation->setState("idle");
			p->accuracy *= glm::pow(0.25f, elapsedTime);
		}
	}

	for (auto z : zombies)
	{
		glm::vec2 oldPos = z->position;
		z->position += glm::normalize(z->target->position - z->position) * elapsedTime * 100.0f;
		blib::math::Line ray(z->position, oldPos);
		glm::vec2 point;
		blib::math::Line hitLine;
		for (auto o : objects)
			if (o.intersects(ray, point, hitLine))
			{
				float overShoot = glm::length(z->position - point);
				z->position += overShoot * -hitLine.normal();
				break;
			}

		for (auto p : players)
		{
			if (glm::distance(p->position, z->position) < 10)
				p->alive = false;
		}

		while (!z->target->alive)
			z->target = players[rand() % players.size()];



		z->direction = glm::degrees(atan2(z->target->position.y - z->position.y, z->target->position.x - z->position.x));
		z->zombieSprite->setState("walk");
		z->zombieSprite->update(elapsedTime * 0.1f);
	}



}

void ZombieSurvival::draw()
{
	spriteBatch->begin();
	lineBatch->begin();
	spriteBatch->draw(backSprite, glm::mat4());


	blib::RenderState state = lineBatch->renderState;
	state.activeVbo = NULL;
	state.activeFbo = visionFbo;
	state.stencilTestEnabled = true;
	renderer->clear(glm::vec4(0, 0, 0, 1.0), blib::Renderer::Color | blib::Renderer::Stencil, state);
	for (auto p : players)
	{
		glm::vec2 lightPoint(p->position);

		std::vector<blib::VertexP2C4> verts;
		for (auto o : objects)
		{
			blib::math::Line prevRay;
			bool first = true;
			for (int i = 0; i < o.size() + 1; i++)
			{
				const glm::vec2& v = o[i%o.size()];
				blib::math::Line ray(v, v + 50.0f * (v - lightPoint));
				if (!first)
				{
					verts.push_back(blib::VertexP2C4(ray.p1, glm::vec4(0, 0, 0, 0.9f)));
					verts.push_back(blib::VertexP2C4(ray.p2, glm::vec4(0, 0, 0, 0.9f)));
					verts.push_back(blib::VertexP2C4(prevRay.p1, glm::vec4(0, 0, 0, 0.9f)));

					verts.push_back(blib::VertexP2C4(prevRay.p1, glm::vec4(0, 0, 0, 0.9f)));
					verts.push_back(blib::VertexP2C4(prevRay.p2, glm::vec4(0, 0, 0, 0.9f)));
					verts.push_back(blib::VertexP2C4(ray.p2, glm::vec4(0, 0, 0, 0.9f)));
				}
				prevRay = ray;
				first = false;
			}
		}


		renderer->clear(glm::vec4(1, 1, 1, 0.5f), blib::Renderer::Stencil, state);
		state.stencilWrite = true;
		renderer->drawTriangles(verts, state);
		state.stencilWrite = false;
		state.blendEnabled = false;
		
		//for more difficulty, use cone here instead of full screen
		std::vector<blib::VertexP2C4> verts2{
			blib::VertexP2C4(glm::vec2(0, 0), glm::vec4(1, 1, 1, 0.0)),
			blib::VertexP2C4(glm::vec2(1920, 0), glm::vec4(1, 1, 1, 0.0)),
			blib::VertexP2C4(glm::vec2(0, 1080), glm::vec4(1, 1, 1, 0.0)),

			blib::VertexP2C4(glm::vec2(1920, 1080), glm::vec4(1, 1, 1, 0.0)),
			blib::VertexP2C4(glm::vec2(1920, 0), glm::vec4(1, 1, 1, 0.0)),
			blib::VertexP2C4(glm::vec2(0, 1080), glm::vec4(1, 1, 1, 0.0))
		};

		renderer->drawTriangles(verts2, state);
	}


	for (auto z : zombies)
		z->zombieSprite->draw(*spriteBatch, blib::math::easyMatrix(z->position, z->direction-90, 0.5f));




	spriteBatch->draw(visionFbo, blib::math::easyMatrix(glm::vec2(0,1080), 0, glm::vec2(1,-1)));



	for (auto p : players)
	{
		if (!p->alive)
			continue;
		p->playerAnimation->draw(*spriteBatch, blib::math::easyMatrix(p->position, p->rotation - 90, 0.5f));
		lineBatch->draw(p->position, p->position + 200.0f * blib::util::fromAngle(glm::radians(p->rotation + p->accuracy)));
		lineBatch->draw(p->position, p->position + 200.0f * blib::util::fromAngle(glm::radians(p->rotation - p->accuracy)));
	}
	spriteBatch->end();
	lineBatch->end();


	for (auto p : objects)
	{
	//	lineBatch->draw(p, glm::vec4(0, 0, 1, 1));
	}








}

blib::Texture* ZombieSurvival::getTitleImage()
{
	return NULL;
}
