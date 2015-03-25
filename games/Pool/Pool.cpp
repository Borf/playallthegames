#include "Pool.h"
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


Pool::Pool()
{
	this->totalScore = 10;
	world = NULL;
}

std::string Pool::getName()
{
	return "Pool";
}

std::string Pool::getInstructions()
{
	return "Put balls in holes";
}

void Pool::loadResources()
{
	blib::Box2DDebug::getInstance()->init(lineBatch, renderer);
	blib::Box2DDebug::getInstance()->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit);


	pocketSprite = resourceManager->getResource<blib::Texture>("assets/games/pool/pocket.png");
	ballSprite = resourceManager->getResource<blib::Texture>("assets/games/pool/ball.png");
	halfBallSprite = resourceManager->getResource<blib::Texture>("assets/games/pool/halfball.png");
	cueSprite = resourceManager->getResource<blib::Texture>("assets/games/pool/cue.png");
	whitePixel = resourceManager->getResource<blib::Texture>("assets/textures/whitepixel.png");

	font = resourceManager->getResource<blib::Font>("menu");

}

void Pool::start(Difficulty difficulty)
{
	if (world)
		delete world;
	balls.clear();
	holes.clear();

	world = new b2World(glm::vec2(0,0));
	world->SetDebugDraw(blib::Box2DDebug::getInstance());

	std::vector<glm::vec2> verts;

	verts.push_back(glm::vec2(-20, 10));
	verts.push_back(glm::vec2(20, 10));
	verts.push_back(glm::vec2(20, -10));
	verts.push_back(glm::vec2(-20, -10));

	verts = addHole(verts, glm::vec2(-20 + 0.5f * pocketSize, 10 - 0.5f * pocketSize), pocketSize);
	verts = addHole(verts, glm::vec2(20 - 0.5f * pocketSize, 10 - 0.5f * pocketSize), pocketSize);
	verts = addHole(verts, glm::vec2(-20 + 0.5f * pocketSize, -10 + 0.5f * pocketSize), pocketSize);
	verts = addHole(verts, glm::vec2(20 - 0.5f * pocketSize, -10 + 0.5f * pocketSize), pocketSize);
	verts = addHole(verts, glm::vec2(0, 10 - 0.5f * pocketSize), pocketSize);
	verts = addHole(verts, glm::vec2(0, -10 + 0.5f * pocketSize), pocketSize);



	std::vector<b2Vec2> b2Verts;
	for (size_t i = 0; i < verts.size(); i++)
		b2Verts.push_back(verts[i]);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, 0.0f);
	b2Body* body = world->CreateBody(&groundBodyDef);

	b2ChainShape chain;
	chain.CreateLoop(&b2Verts[0], b2Verts.size());

	b2FixtureDef fixtureDef;
	fixtureDef.restitution = 1.0f;
	fixtureDef.shape = &chain;
	body->CreateFixture(&fixtureDef);




	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y <= x; y++)
		{
			b2BodyDef ballDef;
			b2FixtureDef fixtureDef;
			b2CircleShape ballShape;
			ballShape.m_radius = 0.5f;
			fixtureDef.restitution = 1.0f;
			fixtureDef.shape = &ballShape;
			ballDef.type = b2_dynamicBody;
			ballDef.linearDamping = 0.99f;
			ballDef.angularDamping = 0.1f;// 1.5f;
			ballDef.bullet = true;
			ballDef.position = b2Vec2(5 + x*1.0f, y*1.0f - 0.5f * x);

			b2Body* ball = world->CreateBody(&ballDef);
			ball->CreateFixture(&fixtureDef);

			balls.push_back(std::pair<b2Body*, PoolPlayer*>(ball, NULL));
		}
	}


	for (size_t i = 0; i < players.size(); i++)
	{
		b2BodyDef ballDef;
		b2FixtureDef fixtureDef;
		b2CircleShape ballShape;
		ballShape.m_radius = 0.5f;
		fixtureDef.restitution = 1.0f;
		fixtureDef.shape = &ballShape;
		ballDef.type = b2_dynamicBody;
		ballDef.linearDamping = 0.99f;
		ballDef.angularDamping = 0.1f;
		ballDef.bullet = true;
		ballDef.position = b2Vec2(-15, -7.5f + i * (15.0f / players.size()));

		b2Body* ball = world->CreateBody(&ballDef);
		ball->CreateFixture(&fixtureDef);

		players[i]->ball = ball;
	}
	world->SetContactListener(this);
	



}

void Pool::update(float elapsedTime)
{
	world->Step(elapsedTime, 5, 5);


	for (size_t i = 0; i < players.size(); i++)
	{
		if (glm::length(players[i]->joystick.leftStick) > 0.1)
			players[i]->direction = glm::degrees(atan2(players[i]->joystick.leftStick.y, players[i]->joystick.leftStick.x));

		if (players[i]->joystick.a && !players[i]->prevJoystick.a && players[i]->ball->GetLinearVelocity().Length() < 0.1)
			players[i]->ball->ApplyLinearImpulse(100.0f * blib::util::fromAngle(glm::radians(players[i]->direction)), players[i]->ball->GetWorldCenter());
	}


	for (int i = 0; i < (int)balls.size(); i++)
	{
		for (size_t ii = 0; ii < holes.size(); ii++)
		{
			if (glm::length(holes[ii] - balls[i].first->GetPosition().operator glm::vec2()) < pocketSize + 0.25f && balls[i].second)
			{
				balls[i].second->score++;
				world->DestroyBody(balls[i].first);
				balls.erase(balls.begin() + i);
				i--;
				break;
			}
		}
	}




}

void Pool::draw()
{
	glm::mat4 transform;
	transform = glm::translate(transform, glm::vec3(settings->center, 0));
	transform = glm::scale(transform, glm::vec3(40, 40, 1));
	transform *= settings->scaleMatrix;

	spriteBatch->begin(transform);

	spriteBatch->draw(whitePixel, blib::math::easyMatrix(whitePixel, blib::math::Rectangle(glm::vec2(-21.5, -11.5), glm::vec2(21.5, 11.5))), blib::Color::darkBrown);
	spriteBatch->draw(whitePixel, blib::math::easyMatrix(whitePixel, blib::math::Rectangle(glm::vec2(-21.3, -11.3), glm::vec2(21.3, 11.3))), blib::Color::brown);
	spriteBatch->draw(whitePixel, blib::math::easyMatrix(whitePixel, blib::math::Rectangle(glm::vec2(-20, -10), glm::vec2(20, 10))), blib::Color::darkGreen);
	spriteBatch->draw(whitePixel, blib::math::easyMatrix(whitePixel, blib::math::Rectangle(glm::vec2(-19.5, -9.5), glm::vec2(19.5, 9.5))), blib::Color::green);


	spriteBatch->draw(pocketSprite, blib::math::easyMatrix(glm::vec2(-20 + 0.5f * pocketSize, 10 - 0.5f * pocketSize), 0, pocketSize / glm::vec2(pocketSprite->originalWidth, pocketSprite->originalHeight)*2.0f), pocketSprite->center);
	spriteBatch->draw(pocketSprite, blib::math::easyMatrix(glm::vec2(20 - 0.5f * pocketSize, 10 - 0.5f * pocketSize), 0, pocketSize / glm::vec2(pocketSprite->originalWidth, pocketSprite->originalHeight)*2.0f), pocketSprite->center);
	spriteBatch->draw(pocketSprite, blib::math::easyMatrix(glm::vec2(-20 + 0.5f * pocketSize, -10 + 0.5f * pocketSize), 0, pocketSize / glm::vec2(pocketSprite->originalWidth, pocketSprite->originalHeight)*2.0f), pocketSprite->center);
	spriteBatch->draw(pocketSprite, blib::math::easyMatrix(glm::vec2(20 - 0.5f * pocketSize, -10 + 0.5f * pocketSize), 0, pocketSize / glm::vec2(pocketSprite->originalWidth, pocketSprite->originalHeight)*2.0f), pocketSprite->center);
	spriteBatch->draw(pocketSprite, blib::math::easyMatrix(glm::vec2(0, 10 - 0.5f*pocketSize), 0, pocketSize / glm::vec2(pocketSprite->originalWidth, pocketSprite->originalHeight)*2.0f), pocketSprite->center);
	spriteBatch->draw(pocketSprite, blib::math::easyMatrix(glm::vec2(0, -10 + 0.5f * pocketSize), 0, pocketSize / glm::vec2(pocketSprite->originalWidth, pocketSprite->originalHeight)*2.0f), pocketSprite->center);


	for (size_t i = 0; i < balls.size(); i++)
	{
		spriteBatch->draw(ballSprite, blib::math::easyMatrix(0.5f * balls[i].first->GetPosition(), glm::degrees(balls[i].first->GetAngle()), 0.5f / glm::vec2(ballSprite->originalWidth, ballSprite->originalHeight)) * 2.0f, ballSprite->center, balls[i].second == NULL ? blib::Color::white : balls[i].second->participant->color);
		spriteBatch->draw(halfBallSprite, blib::math::easyMatrix(0.5f * balls[i].first->GetPosition(), glm::degrees(balls[i].first->GetAngle()), 0.5f / glm::vec2(ballSprite->originalWidth, ballSprite->originalHeight)) * 2.0f, ballSprite->center, blib::Color::red);
	}

	for (size_t i = 0; i < players.size(); i++)
	{
		spriteBatch->draw(ballSprite, blib::math::easyMatrix(0.5f * players[i]->ball->GetPosition(), glm::degrees(players[i]->ball->GetAngle()), 0.5f / glm::vec2(ballSprite->originalWidth, ballSprite->originalHeight)) * 2.0f, ballSprite->center, players[i]->participant->color);

		if (players[i]->ball->GetLinearVelocity().Length() < 0.1)
			spriteBatch->draw(cueSprite, blib::math::easyMatrix(0.5f * players[i]->ball->GetPosition() + (-3.0f + glm::cos((float)blib::util::Profiler::getAppTime()*15)) * 0.15f * blib::util::fromAngle(glm::radians(players[i]->direction)), players[i]->direction, 0.01f) * 2.0f, glm::vec2(256, 5));
	}

	

	spriteBatch->end();

	spriteBatch->begin();
	for (size_t i = 0; i < players.size(); i++)
		spriteBatch->draw(font, blib::util::toString(players[i]->score), blib::math::easyMatrix(glm::vec2(10, 64 * i), 0, 1), players[i]->participant->color);


	spriteBatch->end();


	/*

	lineBatch->begin(transform);
	world->DrawDebugData();
	lineBatch->end();*/

}

blib::Texture* Pool::getTitleImage()
{
	throw std::exception("The method or operation is not implemented.");
}

std::vector<glm::vec2> Pool::addHole(const std::vector<glm::vec2> &verts, glm::vec2 pos, float radius)
{
	holes.push_back(pos);

	ClipperLib::Polygon table;
	ClipperLib::Polygon pocket;

	for (size_t i = 0; i < verts.size(); i++)
		table.push_back(verts[i]);


	int detail = 40;

	for (int i = 0; i < detail; i++)
	{
		pocket.push_back(pos + radius * blib::util::fromAngle(i * blib::math::pif / (detail / 2)));
	}


	ClipperLib::Clipper clipper;
	clipper.AddPolygon(table, ClipperLib::ptSubject);
	clipper.AddPolygon(pocket, ClipperLib::ptClip);


	ClipperLib::Polygons solution;
	clipper.Execute(ClipperLib::ctUnion, solution);

	std::vector<glm::vec2> ret;
	for (size_t i = 0; i < solution[0].size(); i++)
	{
		ret.push_back(solution[0][i]);
	}

	return ret;
}

void Pool::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();
	
	if (blib::linq::count(players, [&](PoolPlayer* p) { return p->ball == bodyA || p->ball == bodyB; }) == 1)
	{
		PoolPlayer* player = blib::linq::firstOrDefault(players, [&](PoolPlayer* p) { return p->ball == bodyA || p->ball == bodyB; }, (PoolPlayer*)NULL);

		for (size_t i = 0; i < balls.size(); i++)
		{
			if (balls[i].first == bodyA || balls[i].first == bodyB)
			{
				balls[i].second = player;
			}
		}
	}
	//maybe todo: if 2 non-player-balls collide, make the one with the lower speed owned by the one with the higher speed



}

