#include "Tanks.h"

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/Box2DDebug.h>
#include <blib/LineBatch.h>
#include <blib/Util.h>
#include <blib/Color.h>
#include <blib/Math.h>
#include <blib/ParticleSystem.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <Box2D/Box2D.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

Tanks::Tanks()
{
	world = NULL;
}

std::string Tanks::getName()
{
	return "Tanks";
}

std::string Tanks::getInstructions()
{
	return "Shoot the others";
}

void Tanks::loadResources()
{
	back = resourceManager->getResource<blib::Texture>("assets/games/Tanks/back.png");
	tankSprite = resourceManager->getResource<blib::Texture>("assets/games/Tanks/tank.png");
	turretSprite = resourceManager->getResource<blib::Texture>("assets/games/Tanks/turret.png");
	bulletSprite = resourceManager->getResource<blib::Texture>("assets/games/Tanks/bullet.png");
	crateSprite = resourceManager->getResource<blib::Texture>("assets/games/Tanks/obstacle.png");
	blib::Box2DDebug::getInstance()->init(lineBatch, renderer);

	particleSystem = new blib::ParticleSystem(renderer, resourceManager, spriteBatch);
	particleSystem->setTextureFolder("assets/games/Tanks/particles/");

}

void Tanks::start(Difficulty difficulty)
{
	if(world)
		delete world;
	world = new b2World(b2Vec2(0,0));
	bullets.clear();
	crates.clear();

	world->SetDebugDraw(blib::Box2DDebug::getInstance());
	blib::Box2DDebug::getInstance()->SetFlags( b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit );


	for(size_t i = 0; i < players.size(); i++)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = b2Vec2(glm::vec2(8.5f, 4.5)*blib::util::fromAngle(glm::radians((i / (float)players.size()) * 360+45)));
		bodyDef.linearDamping = 5;
		bodyDef.angularDamping = 10;
		//bodyDef.linearVelocity = b2Vec2(-20*blib::util::fromAngle(glm::radians((i / (float)players.size()) * 360+45)));
		players[i]->body = world->CreateBody(&bodyDef);

		b2PolygonShape shape;
		shape.SetAsBox(0.45f,0.45f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 300;
		fixtureDef.restitution = 0;
		fixtureDef.friction = 1;

		players[i]->body->CreateFixture(&fixtureDef);


		b2BodyDef bodyDef2;
		bodyDef2.type = b2_dynamicBody;
		bodyDef2.position = players[i]->body->GetPosition() + b2Vec2(-0.1f, 0.0f);
		bodyDef2.angularDamping = 1;
		players[i]->turret = world->CreateBody(&bodyDef2);

		b2PolygonShape shape2;
		shape2.SetAsBox(0.45f,0.05f);

		b2FixtureDef fixtureDef2;
		fixtureDef2.shape = &shape2;
		fixtureDef2.density = 300;

		players[i]->turret->CreateFixture(&fixtureDef2);



		b2RevoluteJointDef jointDef;
		jointDef.Initialize(players[i]->body, players[i]->turret, players[i]->body->GetWorldCenter() + b2Vec2(0.1f, 0.0f));

		jointDef.motorSpeed = 1;
		jointDef.maxMotorTorque = 99999999.9f;
		jointDef.enableMotor = true;

		players[i]->turretJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);
	}



	for(int i = 0; i < 150; i++)
	{
		glm::vec2 pos;
		do 
		{
			pos = glm::vec2(blib::math::randomFloat(-10,10), blib::math::randomFloat(-5,5));
		} while (blib::linq::any(players, [pos] (TanksPlayer* p) { return glm::length(glm::vec2(p->body->GetPosition().x, p->body->GetPosition().y) - pos) < 1.5f;}));


		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = pos;
		bodyDef.angle = blib::math::randomFloat((float)M_PI*2);
		bodyDef.linearDamping = 3;
		bodyDef.angularDamping = 0.5f;
		b2Body* crate =world->CreateBody(&bodyDef);

		b2PolygonShape shape;
		shape.SetAsBox(0.3f,0.3f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1000;
		fixtureDef.restitution = 0;
		fixtureDef.friction = 200;

		crate->CreateFixture(&fixtureDef);		

		crates.push_back(crate);
	}

	for(int i = 0; i < 100; i++)
		world->Step(1/60.0f, 3, 3);

}

void Tanks::update( float elapsedTime )
{
	particleSystem->update(elapsedTime);
	world->Step(elapsedTime, 3, 3);
	for(auto p : players)
	{
		if(!p->alive)
			continue;
		float angle = p->body->GetAngle();

		glm::vec2 forward = blib::util::fromAngle(angle);
		glm::vec2 side(-forward.y, forward.x);

		p->body->ApplyForce(500.0f*p->joystick.leftStick.y * forward, p->body->GetWorldCenter() + side);
		p->body->ApplyForce(500.0f*p->joystick.rightStick.y * forward, p->body->GetWorldCenter() -side);

		p->targetAngle = blib::util::wrapAngleRad(p->targetAngle + (p->joystick.leftTrigger - p->joystick.rightTrigger) * elapsedTime);


		p->turretJoint->SetMotorSpeed(p->joystick.rightTrigger - p->joystick.leftTrigger);


		if(p->lastShootTime > 0)
			if(p->lastShootTime > elapsedTime)
				p->lastShootTime-=elapsedTime;
			else
				p->lastShootTime = 0;

		if(p->joystick.a && p->lastShootTime == 0)
		{
			b2BodyDef bulletDef;
			bulletDef.type = b2_dynamicBody;
			bulletDef.position = p->turret->GetWorldCenter() + 0.475f * blib::util::fromAngle(p->turret->GetAngle() + (float)M_PI);
			bulletDef.linearVelocity = 7.0f * blib::util::fromAngle(p->turret->GetAngle() + (float)M_PI);
			bulletDef.angle = p->turret->GetAngle();
			b2Body* bullet = world->CreateBody(&bulletDef);

			b2PolygonShape shape;
			shape.SetAsBox(0.06f,0.025f);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &shape;
			fixtureDef.density = 0.001f;
			fixtureDef.restitution = 0;
			fixtureDef.friction = 0;

			bullet->CreateFixture(&fixtureDef);
			p->lastShootTime = 0.5f;

			bullet->SetUserData(p);

			bullets.push_back(bullet);
		}
	}



	for(int i = bullets.size()-1; i >= 0; i--)
	{
		if(bullets[i]->GetPosition().x < -10 || bullets[i]->GetPosition().x > 10 || bullets[i]->GetPosition().y < -10 || bullets[i]->GetPosition().y > 10)
		{
			world->DestroyBody(bullets[i]);
			bullets.erase(bullets.begin()+i);
			continue;
		}
		bool erased = false;
		b2ContactEdge* collision = bullets[i]->GetContactList();
		while(collision && !erased)
		{
			if(!collision->contact->IsTouching())
			{
				collision = collision->next;
				continue;
			}
			if(collision->other->GetUserData()) // bullet-bullet
			{
				world->DestroyBody(bullets[i]);
				bullets.erase(bullets.begin()+i);
				erased = true;
				continue;
			}

			for(auto p : players)
			{
				if(p->body == collision->other || p->turret == collision->other)
				{
					if(p != bullets[i]->GetUserData())
					{
						if (p->alive)
						{
							blib::Emitter* emitter = particleSystem->addEmitter("assets/games/Tanks/particles/explosion.json");
							emitter->position = emitter->prevPosition = glm::vec2(1920 / 2, 1080 / 2) + 100.0f * p->body->GetPosition();
							emitter->life = 0.5;
						}
						p->alive = false;

						world->DestroyBody(bullets[i]);
						bullets.erase(bullets.begin()+i);
						erased = true;
						break;
					}
				}
			}
			if(!erased)
			{
				for(size_t ii = 0; ii < crates.size(); ii++)
				{
					if(crates[ii] == collision->other)
					{
						blib::Emitter* emitter = particleSystem->addEmitter("assets/games/Tanks/particles/miniexplosion.json");
						emitter->position = emitter->prevPosition = glm::vec2(1920 / 2, 1080 / 2) + 100.0f * crates[ii]->GetPosition();
						emitter->life = 0.5;


						world->DestroyBody(crates[ii]);
						crates.erase(crates.begin()+ii);

						world->DestroyBody(bullets[i]);
						bullets.erase(bullets.begin()+i);
						erased = true;
						break;
					}
				}
			}
			if(!erased)
				collision = collision->next;
		}

	}

}

void Tanks::draw()
{
//	glm::vec2 fac(settings->size/glm::vec2(1920,1080));
//	glm::mat4 transform(glm::scale(glm::translate(glm::mat4(), glm::vec3(settings->center, 0)), glm::vec3(100.0f * fac,1)));

	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(back, blib::math::easyMatrix(back, blib::math::Rectangle(0,0,1920, 1080)));

	for(auto p : players)
		spriteBatch->draw(tankSprite, p->getMatrix(settings), tankSprite->center, blib::math::Rectangle(0,0,1,1), p->alive ? p->participant->color : blib::Color::grey);
	for(auto p : players)
		spriteBatch->draw(turretSprite, p->getTurretMatrix(settings), turretSprite->center, blib::math::Rectangle(0,0,1,1), p->alive ? p->participant->color : blib::Color::grey);

	for(auto b : bullets)
	{
		glm::mat4 matrix;
		matrix = glm::translate(matrix, glm::vec3(1920/2,1080/2, 0));
		matrix = glm::translate(matrix, glm::vec3(100*b->GetPosition().x, 100*b->GetPosition().y, 0));
		matrix = glm::rotate(matrix, glm::degrees(b->GetAngle())+90, glm::vec3(0, 0, 1));

		spriteBatch->draw(bulletSprite, matrix, bulletSprite->center);
	}
	for(auto c : crates)
	{
		glm::mat4 matrix;

		float scaleFac = 0.12f;

		matrix = glm::translate(matrix, glm::vec3(1920 / 2, 1080 / 2, 0));
		matrix = glm::translate(matrix, glm::vec3(100*c->GetPosition().x, 100*c->GetPosition().y, 0));
		matrix = glm::rotate(matrix, glm::degrees(c->GetAngle()), glm::vec3(0, 0, 1));
		matrix = glm::scale(matrix, glm::vec3(scaleFac, scaleFac, 0));

		spriteBatch->draw(crateSprite, matrix, crateSprite->center);
	}

	particleSystem->draw(glm::mat4());


	spriteBatch->end();



	/*lineBatch->begin(transform);
	world->DrawDebugData();
	lineBatch->end();*/
}

blib::Texture* Tanks::getTitleImage()
{
	return NULL;
}
