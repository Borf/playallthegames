#include "Tron.h"

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#include <blib/FBO.h>
#include <blib/ResourceManager.h>
#include <blib/Renderer.h>
#include <blib/SpriteBatch.h>

#include <blib/gl/Vertex.h>

#include <glm/gtc/matrix_transform.hpp>
#include <fstream>


Tron::Tron()
{

}

std::string Tron::getName()
{
	return "Tron";
}

std::string Tron::getInstructions()
{
	return "Don't hit anything";
}

void Tron::loadResources()
{
	fbo = resourceManager->getResource<blib::FBO>();
	fbo->setSize(settings->resX, settings->resY);

	whitePixel = resourceManager->getResource<blib::Texture>("assets/games/Tron/white.png");
}

void Tron::start(Difficulty difficulty)
{
	screen = std::vector< std::vector<bool> >(settings->resX, std::vector<bool>(settings->resY, false));

	for(size_t i = 0; i < players.size(); i++)
	{
		players[i]->position.x = 100;
		players[i]->position.y = 100.0f + ((settings->resY - 200) / (players.size() - 1)) * i;
		players[i]->direction = glm::vec2(1,0);
		players[i]->prevPosition = players[i]->position;
	}
	blib::RenderState renderState;
	renderState.activeFbo = fbo;
	renderer->clear(glm::vec4(0.0f,0.0f,0.0f,1), blib::Renderer::Color, renderState);

	speed = 500 * settings->scale;
}

void Tron::update( float elapsedTime )
{
	for(auto player : players)
	{
		if(!player->alive)
			continue;
		glm::vec2 oldPosition = player->position;
		player->prevPosition = player->position;
		player->position += elapsedTime*speed*player->direction;
		
		if(player->position.x < 1 || glm::round(player->position.x) >= settings->resX-1 ||
			player->position.y < 1 || glm::round(player->position.y) >= settings->resY-1 ||
			oldPosition.x < 1 || glm::round(oldPosition.x) >= settings->resX-1 ||
			oldPosition.y < 1 || glm::round(oldPosition.y) >= settings->resY-1
			)
		{
			player->alive = false;
			continue;
		}


		if(glm::round(oldPosition.x) != glm::round(player->position.x) ||
			glm::round(oldPosition.y) != glm::round(player->position.y))
		{
			glm::vec2 oldPosition2 = oldPosition;
			glm::vec2 diff = glm::normalize(player->position - oldPosition);
			while(glm::length(oldPosition - player->position) >= 0.1f)
			{
				if(screen[(int)glm::round(oldPosition.x)][(int)glm::round(oldPosition.y)])
				{
					player->alive = false;
					break;
				}
				oldPosition += 0.1f * diff;
			}

			glm::vec2 normal(glm::normalize(glm::vec2(-diff.y, diff.x)));
			oldPosition = oldPosition2;
			while(glm::length(oldPosition - player->position) >= 0.1 && (glm::round(oldPosition.x) != glm::round(player->position.x) ||
				glm::round(oldPosition.y) != glm::round(player->position.y)))
			{	
				screen[(int)glm::round(oldPosition.x)][(int)glm::round(oldPosition.y)] = true;
				screen[(int)glm::round(oldPosition.x+normal.x)][(int)glm::round(oldPosition.y+normal.y)] = true;
				screen[(int)glm::round(oldPosition.x-normal.x)][(int)glm::round(oldPosition.y-normal.y)] = true;
				oldPosition += 0.1f * glm::normalize(player->direction);
			}

		}
		if(!player->alive)
			continue;


		glm::vec2 dir = player->joystick.leftStick;
		if (fabs(dir.x) > fabs(dir.y))
			dir.y = 0;
		else
			dir.x = 0;
		if(glm::length(dir) > 0.5)
		{
			dir = glm::normalize(dir);
			if (fabs(dir.x) != fabs(player->direction.x) && fabs(dir.y) != fabs(player->direction.y))
				player->direction = dir;
		}

	}

	speed += 50.0f * elapsedTime;
}

void Tron::draw()
{
	spriteBatch->begin(glm::mat4(), fbo);
	for(auto p : players)
	{
		glm::vec2 pos = p->prevPosition;
		glm::vec2 diff = glm::normalize(p->position - pos);
		while(glm::length(pos-p->position) > 1)
		{
			spriteBatch->draw(whitePixel, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos,0)), glm::vec3(3,3,1)), whitePixel->center, blib::math::Rectangle(0,0,1,1), p->participant->color);
			pos += diff;
		}
		spriteBatch->draw(whitePixel, glm::scale(glm::translate(glm::mat4(), glm::vec3(p->position,0)), glm::vec3(3,3,1)), whitePixel->center, blib::math::Rectangle(0,0,1,1), p->participant->color);
	}
	spriteBatch->end();

	spriteBatch->begin();
	spriteBatch->draw(fbo, glm::translate(glm::scale(glm::mat4(), glm::vec3(1,-1,1)), glm::vec3(0, -settings->resY,0)));
	spriteBatch->end();
}

blib::Texture* Tron::getTitleImage()
{
	return NULL;
}
