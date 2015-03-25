#include "CurveFever.h"

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#include <blib/FBO.h>
#include <blib/ResourceManager.h>
#include <blib/Renderer.h>
#include <blib/SpriteBatch.h>

#include <blib/gl/Vertex.h>
#include <blib/Util.h>
#include <blib/Math.h>

#include <glm/gtc/matrix_transform.hpp>
#include <fstream>

#define _USE_MATH_DEFINES
#include <math.h>


CurveFever::CurveFever()
{

}

std::string CurveFever::getName()
{
	return "CurveFever";
}

std::string CurveFever::getInstructions()
{
	return "Don't hit anything";
}

void CurveFever::loadResources()
{
	fbo = resourceManager->getResource<blib::FBO>();
	fbo->setSize(settings->resX, settings->resY);

	whitePixel =	resourceManager->getResource<blib::Texture>("assets/games/CurveFever/white.png");
	head =			resourceManager->getResource<blib::Texture>("assets/games/CurveFever/head.png");
	direction =		resourceManager->getResource<blib::Texture>("assets/games/CurveFever/direction.png");
}

void CurveFever::start(Difficulty difficulty)
{
	screen = std::vector< std::vector<int> >(settings->resX, std::vector<int>(settings->resY, 0));

	for(size_t i = 0; i < players.size(); i++)
	{
		players[i]->position.x = blib::math::randomFloat(100.0f, settings->resX-100.0f);
		players[i]->position.y = blib::math::randomFloat(100.0f, settings->resY-100.0f);
		players[i]->prevPosition = players[i]->position;
		players[i]->direction = blib::math::randomFloat(0, (float)(2 * M_PI));
		players[i]->ink = -2;
	}
	blib::RenderState renderState;
	renderState.activeFbo = fbo;
	renderer->clear(glm::vec4(0.0f,0.0f,0.0f,1), blib::Renderer::Color, renderState);

	spriteBatch->begin(glm::mat4(), fbo);
	for(int x = 0; x < settings->resX; x++)
	{
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(glm::vec2(x, 0)));
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(glm::vec2(x, settings->resY-1)));
	}
	for(int y = 0; y < settings->resY; y++)
	{
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(glm::vec2(0, y)));
		spriteBatch->draw(whitePixel, blib::math::easyMatrix(glm::vec2(settings->resX-1, y)));
	}
	spriteBatch->end();


	speed = 150 * settings->scale;
	gameTime = 0;
}

void CurveFever::update( float elapsedTime )
{
	for(auto player : players)
	{
		if(!player->alive)
			continue;
		
		if(player->position.x < 1 || glm::round(player->position.x) >= settings->resX-1 ||
			player->position.y < 1 || glm::round(player->position.y) >= settings->resY-1)
		{
			player->alive = false;
			continue;
		}


		if(!player->alive)
			continue;
		player->prevPosition = player->position;
		player->position += blib::util::fromAngle(player->direction) * elapsedTime * speed;
		player->direction += player->joystick.leftStick.x * elapsedTime * speed / 50.0f;


		if(player->ink > 0)
		{
			player->trail.push_back(player->position);
			for(int x = (int)player->position.x - 1; x <= player->position.x+1; x++)
				for(int y = (int)player->position.y - 1; y <= player->position.y+1; y++)
				{
					if(x < 0 || y < 0 || x >= settings->resX || y >= settings->resY)
					{
						player->alive = false;
						break;
					}
					if(screen[x][y] == -(player->index+1) || screen[x][y] == 0)
						screen[x][y] = -(player->index+1);
					else
						player->alive = false;
				}
			player->ink -= elapsedTime;
			if(player->ink < 0)
				player->ink = blib::math::randomDouble(-1,-0.25);
		}
		else
		{
			for(int x = (int)player->position.x - 1; x <= player->position.x+1; x++)
				for(int y = (int)player->position.y - 1; y <= player->position.y+1; y++)
				{
					if(x < 0 || y < 0 || x >= settings->resX || y >= settings->resY)
					{
						player->alive = false;
						break;
					}
					if(!(screen[x][y] == -(player->index+1) || screen[x][y] == 0))
						player->alive = false;
				}
			player->ink += elapsedTime;
			if(player->ink > 0)
				player->ink = glm::pow(blib::math::randomDouble(1.0,3),2.0);
		}
	}

	speed += 5.0f * elapsedTime;
	gameTime += elapsedTime;
}

void CurveFever::draw()
{
	spriteBatch->begin(glm::mat4(), fbo);
	for(auto p : players)
	{
		while(p->trail.size() > 15)
		{
			std::list<glm::vec2>::iterator it = p->trail.begin();
			glm::vec2 pos1 = *it;
			it++;
			glm::vec2 pos2 = *it;
			p->trail.pop_front();
			if(glm::length(pos2-pos1) > 10)
				continue;

			glm::vec2 diff = glm::normalize(pos2 - pos1);
			while(glm::length(pos1-pos2) > 1)
			{
				spriteBatch->draw(whitePixel, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos1,0)), glm::vec3(3,3,1)), whitePixel->center, blib::math::Rectangle(0,0,1,1), p->participant->color);
				for(int x = (int)pos1.x - 1; x <= pos1.x+1; x++)
					for(int y = (int)pos1.y - 1; y <= pos1.y+1; y++)
					{
						screen[x][y] = p->index+1;
					}
				pos1 += diff;
			}
			spriteBatch->draw(whitePixel, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos2,0)), glm::vec3(3,3,1)), whitePixel->center, blib::math::Rectangle(0,0,1,1), p->participant->color);
		}
	}
	spriteBatch->end();

	spriteBatch->begin();
	spriteBatch->draw(fbo, glm::translate(glm::scale(glm::mat4(), glm::vec3(1,-1,1)), glm::vec3(0, -settings->resY,0)));
	for(auto p : players)
	{
		if(p->trail.size() > 0)
		{
			int i = 0;
			glm::vec2 prev = *p->trail.begin();
			for(auto v : p->trail)
			{
				float alpha = 1 - (i / (float)p->trail.size());
				glm::vec2 pos1 = prev;
				glm::vec2 pos2 = v;
				if(glm::length(pos2-pos1) > 10)
					continue;
				glm::vec2 diff = glm::normalize(pos2 - pos1);
				while(glm::length(pos1-pos2) > 1)
				{
					spriteBatch->draw(whitePixel, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos1,0)), glm::vec3(3,3,1)), whitePixel->center, blib::math::Rectangle(0,0,1,1), glm::vec4(glm::vec3(p->participant->color), alpha));
					pos1 += diff;
				}
				spriteBatch->draw(whitePixel, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos2,0)), glm::vec3(3,3,1)), whitePixel->center, blib::math::Rectangle(0,0,1,1), glm::vec4(glm::vec3(p->participant->color), alpha));
				i++;
				prev = v;
			}
		}
		spriteBatch->draw(head, glm::scale(glm::translate(glm::mat4(), glm::vec3(p->position,0)), glm::vec3(2,2,1)), head->center, blib::math::Rectangle(0,0,1,1), p->participant->color);
	}
	if(gameTime < 0.5)
		for(auto p : players)
			spriteBatch->draw(direction, blib::math::easyMatrix(p->position, glm::degrees(p->direction)), glm::vec2(0, 32), blib::math::Rectangle(0,0,1,1), glm::vec4(glm::vec3(p->participant->color), 1-2*gameTime));
	spriteBatch->end();

}

blib::Texture* CurveFever::getTitleImage()
{
	return NULL;
}
