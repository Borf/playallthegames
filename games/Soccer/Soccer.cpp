	#include "Soccer.h"
#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/Box2DDebug.h>
#include <blib/math.h>
#include <blib/Util.h>
#include <blib/Color.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"


std::string Soccer::getName()
{
	return "Soccer";
}

std::string Soccer::getInstructions()
{
	return "Don't let others score";
}

std::pair<int, int> Soccer::getPlayerCount()
{
	return std::pair<int, int>(1, 99);
}

void Soccer::loadResources()
{
	blib::Box2DDebug::getInstance()->init(lineBatch, renderer);
	blib::Box2DDebug::getInstance()->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit);


	backgroundTexture = resourceManager->getResource<blib::Texture>("assets/games/Soccer/background.png");
	playerSprite = resourceManager->getResource<blib::Texture>("assets/games/Soccer/player.png");
	player1Sprite = resourceManager->getResource<blib::Texture>("assets/games/Soccer/player1.png");
	player2Sprite = resourceManager->getResource<blib::Texture>("assets/games/Soccer/player2.png");
	ballSprite = resourceManager->getResource<blib::Texture>("assets/games/Soccer/ball.png");
	bottleSprite = resourceManager->getResource<blib::Texture>("assets/games/Soccer/bottle.png");
	circleSprite = resourceManager->getResource<blib::Texture>("assets/games/Soccer/circle.png");
}



Soccer::Soccer()
{
	world = NULL;
	backgroundColor = glm::vec4(0, 0, 0,1);
}

void Soccer::start(Difficulty difficulty)
{
	if (world)
		delete world;

	world = new b2World(glm::vec2(0, 0));
	world->SetDebugDraw(blib::Box2DDebug::getInstance());


	world->SetContactListener(this);

	b2Vec2 levelPoints[] = {
		b2Vec2(-19.2f, -10.8f),
		b2Vec2(19.2f, -10.8f),
		b2Vec2(19.2f, 10.8f),
		b2Vec2(-19.2f, 10.8f)
	};
	b2ChainShape levelShape;
	levelShape.CreateLoop(levelPoints, 4);


	b2BodyDef levelDef;
	levelDef.type = b2_staticBody;
	b2Body* level = world->CreateBody(&levelDef);

	b2FixtureDef levelFixture;
	levelFixture.shape = &levelShape;
	level->CreateFixture(&levelFixture);


	for (auto p : players)
	{
		for (int i = 0; i < 2; i++)
		{
			b2CircleShape playerShape;
			playerShape.m_radius = .5f;
			b2BodyDef bodyDef;
			bodyDef.fixedRotation = true;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position = glm::vec2(15, 9) * (i == 0 ? 1.15f : 0.85f) * blib::util::fromAngle(2 * blib::math::pif / players.size() * p->index);
			bodyDef.linearDamping = 10;
			b2Body* body = world->CreateBody(&bodyDef);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &playerShape;
			fixtureDef.density = 1;
			fixtureDef.friction = 1.0f;
			fixtureDef.restitution = 0;
			fixtureDef.filter.categoryBits = 2;
			fixtureDef.filter.maskBits = 0xffff & ~4;
			body->CreateFixture(&fixtureDef);
			

			p->players[i] = body;
		}

		b2CircleShape playerShape;
		playerShape.m_radius = .2f;
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.angularDamping = 1.0f;
		bodyDef.position = glm::vec2(15,9) * blib::util::fromAngle(2 * blib::math::pif / players.size() * p->index);
		bodyDef.linearDamping = 4;
		p->bottle = world->CreateBody(&bodyDef);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &playerShape;
		fixtureDef.density = 1;
		fixtureDef.friction = 1.0f;
		fixtureDef.restitution = 0;
		fixtureDef.filter.categoryBits = 4;
		fixtureDef.filter.maskBits = 0xffff & ~2;

		p->bottle->CreateFixture(&fixtureDef);
	}


	b2CircleShape ballShape;
	ballShape.m_radius = 0.3f;

	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	
	ball = world->CreateBody(&ballBodyDef);

	b2FixtureDef ballFixture;
	ballFixture.shape = &ballShape;
	ballFixture.density = 0.2f;
	ballFixture.restitution = 0.7f;
	ball->CreateFixture(&ballFixture);


	possessionJoint = NULL;
	possessionPlayer = NULL;

	physTime = 0;
}

void Soccer::update(float elapsedTime)
{
	b2Body* possessionPlayerPrev = possessionPlayer;
	physTime += elapsedTime;
	while (physTime > 1 / 60.0f)
	{
		world->Step(1 / 60.0f, 3, 3);
		physTime -= 1 / 60.0f;
	}

	if (possessionPlayerPrev != possessionPlayer)
	{
		if (possessionJoint)
			world->DestroyJoint(possessionJoint);
		possessionJoint = NULL;
		if (possessionPlayer)
		{
			glm::vec2 diff = ball->GetPosition() - possessionPlayer->GetPosition();
			possessionPlayer->SetTransform(possessionPlayer->GetPosition(), atan2(diff.y, diff.x));

			b2WeldJointDef testJoint;

			testJoint.bodyA = possessionPlayer;
			testJoint.bodyB = ball;

			testJoint.localAnchorA = possessionPlayer->GetLocalCenter() + b2Vec2(0.8f, 0);
			testJoint.localAnchorB = ball->GetLocalCenter();
			possessionJoint = world->CreateJoint(&testJoint);
		}
	}


	for (auto p : players)
	{
		p->players[0]->ApplyForceToCenter(20 * p->joystick.leftStick);
		p->players[1]->ApplyForceToCenter(20 * p->joystick.rightStick);

		if (glm::length(p->joystick.leftStick) > 0.25f)
			p->players[0]->SetTransform(p->players[0]->GetPosition(), atan2(p->joystick.leftStick.y, p->joystick.leftStick.x));
		if (glm::length(p->joystick.rightStick) > 0.25f)
			p->players[1]->SetTransform(p->players[1]->GetPosition(), atan2(p->joystick.rightStick.y, p->joystick.rightStick.x));

		if (((p->joystick.a == 1 && p->prevJoystick.a != 1) || (p->joystick.l == 1 && p->prevJoystick.l != 1)) && possessionJoint)
		{
			if (possessionJoint->GetBodyA() == p->players[0])
			{
				world->DestroyJoint(possessionJoint);
				possessionJoint = NULL;
				possessionPlayer = NULL;
				ball->ApplyForceToCenter(50 * blib::util::fromAngle(p->players[0]->GetAngle()));
			}
			else if (possessionJoint->GetBodyA() == p->players[1])
			{
				world->DestroyJoint(possessionJoint);
				possessionJoint = NULL;
				possessionPlayer = NULL;
				ball->ApplyForceToCenter(50 * blib::util::fromAngle(p->players[1]->GetAngle()));
			}
		}
		if (((p->joystick.b == 1 && p->prevJoystick.b != 1) || (p->joystick.r == 1 && p->prevJoystick.r != 1)) && possessionJoint)
		{
			if (possessionJoint->GetBodyA() == p->players[0])
			{
				world->DestroyJoint(possessionJoint);
				possessionJoint = NULL;
				possessionPlayer = NULL;
				glm::vec2 diff = p->players[1]->GetPosition() - p->players[0]->GetPosition();
				ball->ApplyForceToCenter(50.0f * glm::normalize(diff));
			}
			else if (possessionJoint->GetBodyA() == p->players[1])
			{
				world->DestroyJoint(possessionJoint);
				possessionJoint = NULL;
				possessionPlayer = NULL;
				glm::vec2 diff = p->players[0]->GetPosition() - p->players[1]->GetPosition();
				ball->ApplyForceToCenter(50.0f * glm::normalize(diff));
			}
		}


		if (p->alive && glm::distance(p->bottle->GetPosition().operator glm::vec2(), glm::vec2(15, 9) * blib::util::fromAngle(2 * blib::math::pif / players.size() * p->index)) > 2)
		{
			p->alive = false;
		}



	}

}

void Soccer::draw()
{
	glm::mat4 transform = settings->scaleMatrix;

	transform = glm::translate(transform, glm::vec3(1920 / 2, 1080 / 2, 0));
	transform = glm::scale(transform, glm::vec3(50.0f, 50.0f, 10.0f));


	spriteBatch->begin(transform);
	spriteBatch->draw(backgroundTexture, blib::math::easyMatrix(backgroundTexture, blib::math::Rectangle(glm::vec2(-19.2f,-10.8f), glm::vec2(19.2f,10.8f))));

	for (auto p : players)
	{
		spriteBatch->draw(circleSprite, blib::math::easyMatrix(circleSprite, blib::math::Rectangle(glm::vec2(15, 9) * blib::util::fromAngle(2 * blib::math::pif / players.size() * p->index) - glm::vec2(2.0f, 2.0f), 4.0f, 4.0f)), p->alive ? blib::Color::white : blib::Color::red);
	}


	glm::vec2 ballPos = ball->GetPosition();
	spriteBatch->draw(ballSprite, blib::math::easyMatrix(ballSprite, blib::math::Rectangle(ballPos - glm::vec2(0.3f,0.3f), 0.6f,0.6f)));

	for (auto p : players)
	{
		{
			glm::vec2 playerPos = p->players[0]->GetPosition();
			spriteBatch->draw(playerSprite, blib::math::easyMatrix(playerSprite, blib::math::Rectangle(playerPos - glm::vec2(0.5f, 0.5f), 1.0f, 1.0f)));
			spriteBatch->draw(player1Sprite, blib::math::easyMatrix(player1Sprite, blib::math::Rectangle(playerPos - glm::vec2(0.5f, 0.5f), 1.0f, 1.0f)), p->participant->color);
		}
		{
			glm::vec2 playerPos = p->players[1]->GetPosition();
			spriteBatch->draw(playerSprite, blib::math::easyMatrix(playerSprite, blib::math::Rectangle(playerPos - glm::vec2(0.5f, 0.5f), 1.0f, 1.0f)));
			spriteBatch->draw(player2Sprite, blib::math::easyMatrix(player1Sprite, blib::math::Rectangle(playerPos - glm::vec2(0.5f, 0.5f), 1.0f, 1.0f)), p->participant->color);
		}

		spriteBatch->draw(bottleSprite, blib::math::easyMatrix(playerSprite, blib::math::Rectangle(p->bottle->GetPosition() - glm::vec2(0.2f, 1.4f), 1.5f, 1.5f), p->bottle->GetPosition(), p->bottle->GetAngle()));

	
	}



	spriteBatch->end();

	
	lineBatch->begin(transform);
	world->DrawDebugData();
	lineBatch->end();

}

blib::Texture* Soccer::getTitleImage()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Soccer::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	while (contact)
	{
		if (contact->IsTouching())
		{
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			b2Body* other = NULL;
			if (bodyA == ball)
				other = bodyB;
			if (bodyB == ball)
				other = bodyA;

			for (auto p : players)
			{
				for (int i = 0; i < 2; i++)
					if (p->players[i] == other)
						possessionPlayer = p->players[i];

				if (other == p->bottle)
					p->bottle->ApplyAngularImpulse(100);

			}


		}

		contact = contact->GetNext();
	}
}

