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

	visionFbo = resourceManager->getResource<blib::FBO>(settings->resX, settings->resY);
	visionFbo->depth = false;
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


}

void ZombieSurvival::update(float elapsedTime)
{


	for (auto p : players)
	{
		p->playerAnimation->update(elapsedTime);
		if (!p->alive)
			continue;

		p->position += p->joystick.leftStick * elapsedTime * 200.0f;
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


}

void ZombieSurvival::draw()
{
	spriteBatch->begin();
	lineBatch->begin();
	spriteBatch->draw(backSprite, glm::mat4());

	for (auto p : players)
	{
		p->playerAnimation->draw(*spriteBatch, blib::math::easyMatrix(p->position, p->rotation-90));
		lineBatch->draw(p->position, p->position + 200.0f * blib::util::fromAngle(glm::radians(p->rotation + p->accuracy)));
		lineBatch->draw(p->position, p->position + 200.0f * blib::util::fromAngle(glm::radians(p->rotation - p->accuracy)));
	}


	for (auto p : objects)
	{
		lineBatch->draw(p, glm::vec4(0, 0, 1, 1));
	}


	glm::vec2 lightPoint(players[0]->position);

	std::vector<blib::VertexP2C4> verts;


	for (auto o : objects)
	{
		blib::math::Line prevRay;
		bool first = true;
		for (int i = 0; i < o.size()+1; i++)
		{
			const glm::vec2& v = o[i%o.size()];
			blib::math::Line ray(v, v + 50.0f * (v - lightPoint));
			
		/*	for (auto oo : objects)
			{
				std::vector<std::pair<glm::vec2, blib::math::Line> > collisions;
				if (oo.intersects(ray, &collisions))
				{
					for (auto c : collisions)
					{
						if (glm::distance(c.first, v) < 0.1f)
							continue;
						ray.p2 = c.first;
					}
				}
			}*/

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
			lineBatch->draw(ray, glm::vec4(0, 1, 0, 1));
		}
	}

	
	spriteBatch->end();
	lineBatch->end();

	blib::RenderState state = lineBatch->renderState;
	state.activeVbo = NULL;
	renderer->drawTriangles(verts, state);


}

blib::Texture* ZombieSurvival::getTitleImage()
{
	return NULL;
}
