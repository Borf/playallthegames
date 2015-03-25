#include "PushOut.h"

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/Util.h>
#include <blib/util/Log.h>
#include <blib/Box2DDebug.h>

#include <Box2D/Box2D.h>
#include <glm/gtc/matrix_transform.hpp>

using blib::util::Log;

std::string PushOut::getName()
{
	return "PushOut";
}

std::string PushOut::getInstructions()
{
	return "Push the others out of the ring";
}

PushOut::PushOut()
{
	world = NULL;
}


void PushOut::loadResources()
{
	background = resourceManager->getResource<blib::Texture>("assets/games/PushOut/back.png");
	playerSprite = resourceManager->getResource<blib::Texture>("assets/games/PushOut/ball.png");
	blib::Box2DDebug::getInstance()->init(lineBatch, renderer);
}

void PushOut::start(Difficulty difficulty)
{
	if(world)
		delete world;
	world = new b2World(b2Vec2(0,0));


	world->SetDebugDraw(blib::Box2DDebug::getInstance());
	blib::Box2DDebug::getInstance()->SetFlags( b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit );


	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	b2Body* body = world->CreateBody(&bodyDef);


	b2Vec2 vs[4] = {b2Vec2(-9.6f, -5.4f), 
					b2Vec2(9.6f, -5.4f), 
					b2Vec2(9.6f, 5.4f), 
					b2Vec2(-9.6f, 5.4f)	};
	b2ChainShape edge;
	edge.CreateLoop(vs, 4);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &edge;
	body->CreateFixture(&fixtureDef);




	for(size_t i = 0; i < players.size(); i++)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = b2Vec2(3.5f*blib::util::fromAngle(glm::radians((i / (float)players.size()) * 360+45)));
		bodyDef.linearDamping = 5;
		bodyDef.angularDamping = 1;
		bodyDef.linearVelocity = b2Vec2(-15*blib::util::fromAngle(glm::radians((i / (float)players.size()) * 360+45)));
		players[i]->body = world->CreateBody(&bodyDef);

		b2CircleShape shape;
		shape.m_radius = 0.5f;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.restitution = 2.0f;
		fixtureDef.friction = 0;

		players[i]->body->CreateFixture(&fixtureDef);
	}
	


	physTime = 0;

}

void PushOut::update( float elapsedTime )
{
	physTime += elapsedTime;
	while (physTime > 1 / 60.0f)
	{
		world->Step(1/60.0f, 3, 3);
		physTime -= 1 / 60.0f;
	}

	for(size_t i = 0; i < players.size(); i++)
	{
		players[i]->body->ApplyForceToCenter(elapsedTime * 4000 * players[i]->joystick.leftStick);

		if(players[i]->body->GetWorldCenter().Length() > 5)
			players[i]->alive = false;

	}

}

void PushOut::draw()
{
	glm::vec2 fac(settings->size/glm::vec2(1920,1080));
	glm::mat4 transform(glm::scale(glm::translate(glm::mat4(), glm::vec3(settings->center, 0)), glm::vec3(100.0f * fac,1)));

	spriteBatch->begin();
	spriteBatch->draw(background, glm::scale(glm::mat4(), glm::vec3(fac,1)));
	for(size_t i = 0; i < players.size(); i++)
	{
		glm::mat4 matrix;
		matrix = glm::scale(glm::translate(matrix, glm::vec3(settings->center, 0)), glm::vec3(fac,1));
		matrix = glm::translate(matrix, glm::vec3(100*players[i]->body->GetWorldCenter().x, 100*players[i]->body->GetWorldCenter().y, 0));
		spriteBatch->draw(playerSprite, matrix, playerSprite->center, blib::math::Rectangle(0,0,1,1), (players[i]->alive ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.4f, 0.4f, 0.4f, 0.8f)) * players[i]->participant->color);
	}
	spriteBatch->end();

/*	lineBatch->begin(transform);
	world->DrawDebugData();
	lineBatch->end();*/
}


blib::Texture* PushOut::getTitleImage()
{
	return NULL;
}

