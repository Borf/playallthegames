#include "PewPew.h"
#include <Box2D/Box2D.h>
#include <glm/gtc/matrix_transform.hpp>

#include <blib/util/Profiler.h>
#include <blib/Box2DDebug.h>
#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/Util.h>
#include <blib/ResourceManager.h>
#include <blib/Math.h>
#include <blib/Color.h>
#include <clipper/clipper.hpp>


#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "level.h"
#include "collisionlayers.h"
#include "levelobject.h"


PewPew::PewPew()
{
	world = NULL;
}

std::string PewPew::getName()
{
	return "PewPew";
}

std::string PewPew::getInstructions()
{
	return "Fly to the end";
}

void PewPew::loadResources()
{
	blib::Box2DDebug::getInstance()->init(lineBatch, renderer);
	blib::Box2DDebug::getInstance()->SetFlags(b2Draw::e_shapeBit /*| b2Draw::e_jointBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit*/);


	playerSprite = resourceManager->getResource<blib::Texture>("assets/games/PewPew/textures/player.png");
}

void PewPew::start(Difficulty difficulty)
{
	if (world)
		delete world;

	world = new b2World(glm::vec2(0,0));
	world->SetDebugDraw(blib::Box2DDebug::getInstance());

	world->SetGravity(glm::vec2(0, 9.8));


 	level = new Level("assets/games/PewPew/levels/patg1.ppl", world, resourceManager, renderer);

	LevelObjectBase* levelObject = level->getObject("start");
	glm::vec2 position = levelObject->position + levelObject->size/2.0f;

	for (size_t i = 0; i < players.size(); i++)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set((position.x) / 128 + blib::math::randomFloat(-1, 1), (position.y) / 128.0f + blib::math::randomFloat(-0.01f, 0.01f));
		bodyDef.angle = glm::radians(0.0f);
		bodyDef.angularVelocity = 0;
		bodyDef.allowSleep = false;
		bodyDef.gravityScale = 0;
		bodyDef.linearDamping = 0.99f;

		players[i]->body = world->CreateBody(&bodyDef);
/*		body->SetUserData(new GravityObjectCollision(this));*/

		b2CircleShape shape;
		shape.m_radius = 0.2f;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;

		fixtureDef.density = 10.0f;//non-0 = static
		fixtureDef.friction = 1.0f;
		fixtureDef.restitution = 0.7f;
		fixtureDef.filter.categoryBits = COL_OBJECTS;
		fixtureDef.filter.maskBits = COL_ALL - COL_BUTTERFLY - COL_SCARF;
		players[i]->body->CreateFixture(&fixtureDef);
		players[i]->body->SetBullet(true);

	}

	for (int i = 0; i < 100; i++)
		world->Step(0.1f, 10, 10);


//	world->SetContactListener(this);
	
	time = 0;


}



void PewPew::update(float elapsedTime)
{
	time += elapsedTime;
	while (time > (1 / 60.0f))
	{
		world->Step(1/60.0f, 5, 5);
		time -= (1 / 60.0f);
	}
	
	for (size_t i = 0; i < players.size(); i++)
	{
		players[i]->body->ApplyLinearImpulse(elapsedTime * 10 * players[i]->joystick.leftStick, players[i]->body->GetPosition());

		if (glm::length(players[i]->joystick.leftStick) > 0.1f)
		{
			float angle = atan2(players[i]->joystick.leftStick.y, players[i]->joystick.leftStick.x);
			players[i]->body->SetTransform(players[i]->body->GetPosition(), glm::degrees(angle));
		}
	}

	level->update(gameTime, elapsedTime, players);

}

void PewPew::draw()
{
	blib::math::Rectangle cameraField(glm::vec2(9999, 99999), glm::vec2(-9999, -9999));

	for (auto p : players)
	{
		cameraField.topleft = glm::min(cameraField.topleft, glm::vec2(p->body->GetWorldCenter().x, p->body->GetWorldCenter().y));
		cameraField.bottomright = glm::max(cameraField.bottomright, glm::vec2(p->body->GetWorldCenter().x, p->body->GetWorldCenter().y));
	}

	glm::vec2 scale = cameraField.size();
	float s = glm::pow(glm::max(scale.x, scale.y), 0.55f);

	s = glm::max(1.75f, s);

	glm::vec2 camera = 128.0f * cameraField.center();
	glm::mat4 cameraMatrix;
	cameraMatrix = cameraMatrix * settings->scaleMatrix;
	cameraMatrix = glm::translate(cameraMatrix, glm::vec3(1920 / 2, 1080 / 2, 0));
	cameraMatrix = glm::scale(cameraMatrix, glm::vec3(2 / s, 2 / s, 1));
	cameraMatrix = glm::translate(cameraMatrix, glm::vec3(-camera, 0));

	spriteBatch->begin(cameraMatrix);
	level->draw(spriteBatch);
	level->drawObjects(spriteBatch);


	for (auto p : players)
	{
		spriteBatch->draw(playerSprite, blib::math::easyMatrix(128.0f * p->body->GetWorldCenter(), p->body->GetAngle(), 0.5f), playerSprite->center, p->participant->color);
	}


	level->drawOverlay(spriteBatch);


	spriteBatch->end();

	/*cameraMatrix = glm::scale(cameraMatrix, glm::vec3(128.0f, 128.0f, 0));
	lineBatch->begin(cameraMatrix);
	world->DrawDebugData();
	lineBatch->end();*/

}

blib::Texture* PewPew::getTitleImage()
{
	throw std::exception("The method or operation is not implemented.");
}


bool PewPew::hasWinner()
{
	LevelObjectBase* finish = level->getObject("finish");
	return blib::linq::any(players, [finish](PewPewPlayer* p)
	{
		return finish->contains(128.0f * p->body->GetWorldCenter());			
	});
}

std::list<Player*> PewPew::getWinners()
{
	LevelObjectBase* finish = level->getObject("finish");
	return blib::linq::where<std::list<Player*>>(players, [finish](PewPewPlayer* p)
	{
		return finish->contains(128.0f * p->body->GetWorldCenter());
	});


}

