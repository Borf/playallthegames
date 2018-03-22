#include "Tanks.h"

#define SCALE 50

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/Box2DDebug.h>
#include <blib/LineBatch.h>
#include <blib/Util.h>
#include <blib/Color.h>
#include <blib/Math.h>
#include <blib/ParticleSystem.h>
#include <blib/audio/AudioManager.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <Box2D/Box2D.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

Tanks::Tanks()
{
	world = NULL;
	controls[ControllerButton::ButtonA] = "Shoot";
	controls[ControllerButton::TriggersLeft] = "Rotate turret";
	controls[ControllerButton::TriggersRight] = "Rotate turret";
	controls[ControllerButton::ThumbstickLeft] = "Move left track";
	controls[ControllerButton::ThumbstickRight] = "Move right track";
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
	tankSprite = resourceManager->getResource<blib::SpriteSheet>("assets/games/Tanks/hull");
	turretSprite = resourceManager->getResource<blib::SpriteSheet>("assets/games/Tanks/turret");
	bulletSprite = resourceManager->getResource<blib::Texture>("assets/games/Tanks/bullet.png");
	crateSprite = resourceManager->getResource<blib::Texture>("assets/games/Tanks/barrel.png");
	wallSprite = resourceManager->getResource<blib::Texture>("assets/games/Tanks/crate.png");
	blib::Box2DDebug::getInstance()->init(lineBatch, renderer);

	particleSystem = new blib::ParticleSystem(renderer, resourceManager, spriteBatch);
	particleSystem->resizeGl(settings->resX, settings->resY,0,0);
	particleSystem->setTextureFolder("assets/games/Tanks/particles/");

	boom = audioManager->loadSample("assets/games/Tanks/boom.wav");
	boom->canOnlyPlayOnce = false;

	explode = audioManager->loadSample("assets/games/Tanks/explode.wav");
	explode->canOnlyPlayOnce = false;

	shot = audioManager->loadSample("assets/games/Tanks/shot.wav");
	shot->canOnlyPlayOnce = false;

}

std::string level;
void Tanks::start()
{
	level = "";
	level += "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	level += "x            x               x                             x";
	level += "x            x               x                             x";
	level += "x            x               x                             x";
	level += "x            x               x                             x";
	level += "x            x               x             x               x";
	level += "x                            x             x               x";
	level += "x                            x             xxxxxxxxxxxxxxxxx";
	level += "x                            x             x               x";
	level += "xxxxxxxxxxxxxxxxxxxxxx       x             x               x";
	level += "x                    x       x             x               x";
	level += "x                    x       x             x               x";
	level += "x                    x       xxxxxx        x               x";
	level += "x                    x                                     x";
	level += "x                    x                                     x";
	level += "x               xxxxxx                                     x";
	level += "x                                                          x";
	level += "x                                                          x";
	level += "x                                   xxxxxxxxxxxx           x";
	level += "x                                   x                      x";
	level += "x                                   x                      x";
	level += "xxx      xxxxxxxxxxxxxxxxx          x                      x";
	level += "x                                   x                      x";
	level += "x                                   x         xxxxxxxxxxxxxx";
	level += "x                                   x                      x";
	level += "x                   xxxxxxxxxxxxxxxxx                      x";
	level += "xxx                 x            x                         x";
	level += "x                   x            x                         x";
	level += "x      x            x            xxxxxxxxxxxx              x";
	level += "x      x            x                                      x";
	level += "x      x                                                   x";
	level += "x      x                                                   x";
	level += "x      x                                                   x";
	level += "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";


	if(world)
		delete world;
	world = new b2World(b2Vec2(0,0));
	bullets.clear();
	crates.clear();

	world->SetDebugDraw(blib::Box2DDebug::getInstance());
	blib::Box2DDebug::getInstance()->SetFlags( b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit);


	for(size_t i = 0; i < players.size(); i++)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = b2Vec2(glm::vec2(8.5f, 4.5)*blib::util::fromAngle(glm::radians((i / (float)players.size()) * 360+45)));
		bodyDef.linearDamping = 4;
		bodyDef.angularDamping = 20;
		//bodyDef.linearVelocity = b2Vec2(-20*blib::util::fromAngle(glm::radians((i / (float)players.size()) * 360+45)));
		players[i]->body = world->CreateBody(&bodyDef);

		b2PolygonShape shape;
		shape.SetAsBox(0.4f,0.2f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1000;
		fixtureDef.restitution = 0;
		fixtureDef.friction = 1;

		players[i]->body->CreateFixture(&fixtureDef);


		b2BodyDef bodyDef2;
		bodyDef2.type = b2_dynamicBody;
		bodyDef2.position = players[i]->body->GetPosition() + b2Vec2(-0.35f, 0.0f);
		bodyDef2.angularDamping = 1;
		players[i]->turret = world->CreateBody(&bodyDef2);

		b2PolygonShape shape2;
		shape2.SetAsBox(0.45f,0.05f);

		b2FixtureDef fixtureDef2;
		fixtureDef2.shape = &shape2;
		fixtureDef2.density = 300;
		if (difficulty == Difficulty::Wtf)
			fixtureDef2.density = 900;


		players[i]->turret->CreateFixture(&fixtureDef2);



		b2RevoluteJointDef jointDef;
		jointDef.Initialize(players[i]->body, players[i]->turret, players[i]->body->GetWorldCenter() + b2Vec2(0.0f, 0.0f));

		jointDef.motorSpeed = 1;
		jointDef.maxMotorTorque = 99999999.9f;
		jointDef.enableMotor = true;

		players[i]->turretJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);


	//	players[i]->body->SetTransform(players[i]->body->GetPosition(), glm::radians((i / (float)players.size()) * 360 + 45+180));

	//	players[i]->turret->SetTransform(players[i]->body->GetPosition(), glm::radians((i / (float)players.size()) * 360 + 45+90));

	}

	for (int ii = 0; ii < 34; ii++)
	{
		for (int i = 0; i < 60; i++)
		{
			if (level[ii * 60 + i] == 'x')
			{
				b2BodyDef bodyDef;
				bodyDef.type = b2_staticBody;
				bodyDef.position = b2Vec2((i-30) * .64 + .32, (ii-17) * .64 + .32 + .68);
				bodyDef.angle = 0;
				b2Body* crate = world->CreateBody(&bodyDef);

				b2PolygonShape shape;
				shape.SetAsBox(0.32f, 0.32f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &shape;
				fixtureDef.density = 1000;
				fixtureDef.restitution = 0;
				fixtureDef.friction = 200;

				crate->CreateFixture(&fixtureDef);
			
			}
		}
	}



	int count = 100;
	if (difficulty == Difficulty::Wtf)
		count = 500;
	for(int i = 0; i < count; i++)
	{
		glm::vec2 pos;
		do 
		{
			pos = glm::vec2(blib::math::randomFloat(-20,20), blib::math::randomFloat(-10,10));
		} while (blib::linq::any(players, [pos] (TanksPlayer* p) { return glm::length(glm::vec2(p->body->GetPosition().x, p->body->GetPosition().y) - pos) < 1.5f;}));


		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = pos;
		//bodyDef.angle = blib::math::randomFloat((float)M_PI*2);
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
	particleSystem->clear();
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

		float force = 500.0f;
		if (difficulty == Difficulty::Wtf)
			force = 2000.0f;

		p->body->ApplyForce(force*p->joystick.leftStick.y * forward, p->body->GetWorldCenter() + side, true);
		p->body->ApplyForce(force*p->joystick.rightStick.y * forward, p->body->GetWorldCenter() -side, true);

		float turnSpeed = 1.0f;
		if (difficulty == Difficulty::Easy)
			turnSpeed = 1.5f;
		if (difficulty == Difficulty::Hard)
			turnSpeed = 0.5f;
		if (difficulty == Difficulty::Cruel)
			turnSpeed = 0.1f;
		if (difficulty == Difficulty::Wtf)
			turnSpeed = 25.0f;


		p->targetAngle = blib::util::wrapAngleRad(p->targetAngle + (p->joystick.leftTrigger - p->joystick.rightTrigger) * elapsedTime);


		p->turretJoint->SetMotorSpeed(turnSpeed * (p->joystick.rightTrigger - p->joystick.leftTrigger));


		if(p->lastShootTime > 0)
			if(p->lastShootTime > elapsedTime)
				p->lastShootTime-=elapsedTime;
			else
				p->lastShootTime = 0;


		if((p->joystick.a || p->joystick.l || p->joystick.r) && p->lastShootTime == 0)
		{
			b2BodyDef bulletDef;
			bulletDef.type = b2_dynamicBody;
			bulletDef.position = p->turret->GetWorldCenter() + 0.5f * blib::util::fromAngle(p->turret->GetAngle() + (float)M_PI);
			bulletDef.linearVelocity = 7.0f * blib::util::fromAngle(p->turret->GetAngle() + (float)M_PI);
			bulletDef.angle = p->turret->GetAngle();
			bulletDef.angularDamping = 1;
			//bulletDef.fixedRotation = true;
			b2Body* bullet = world->CreateBody(&bulletDef);

			//b2PolygonShape shape;
			//shape.SetAsBox(0.06f,0.025f);

			b2CircleShape shape;
			shape.m_radius = 0.025f;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &shape;
			fixtureDef.density = 0.001f;
			fixtureDef.restitution = 1.0f;
			fixtureDef.friction = 1;

			bullet->CreateFixture(&fixtureDef);
			p->lastShootTime = 0.5f;

			if (difficulty == Difficulty::Hard)
				p->lastShootTime = 1;
			if (difficulty == Difficulty::Cruel)
				p->lastShootTime = 2;

			if (difficulty == Difficulty::Wtf)
				p->lastShootTime = 0.05f;

			bullet->SetUserData(p);

			bullets.push_back(bullet);
			shot->play();
		}
	}



	for(int i = bullets.size()-1; i >= 0; i--)
	{
		/*if(bullets[i]->GetPosition().x < -20 || bullets[i]->GetPosition().x > 10 || bullets[i]->GetPosition().y < -10 || bullets[i]->GetPosition().y > 10)
		{
			world->DestroyBody(bullets[i]);
			bullets.erase(bullets.begin()+i);
			continue;
		}*/
		bool erased = false;
		bool skip = false;
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
					skip = true;
					if(p != bullets[i]->GetUserData())
					{
						if (p->alive)
						{
							blib::Emitter* emitter = particleSystem->addEmitter("assets/games/Tanks/particles/explosion.json");
							emitter->position = emitter->prevPosition = 50.0f * p->body->GetPosition();
							emitter->life = 0.5;
							explode->play();
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
						emitter->position = emitter->prevPosition = 50.0f * crates[ii]->GetPosition();
						emitter->life = 0.5;
						boom->play();


						world->DestroyBody(crates[ii]);
						crates.erase(crates.begin()+ii);

						world->DestroyBody(bullets[i]);
						bullets.erase(bullets.begin()+i);
						erased = true;
						break;
					}
				}
			}

			if (!skip && !erased)
			{
				if (difficulty != Difficulty::Wtf || rand() % 100 < 10)
				{
					blib::Emitter* emitter = particleSystem->addEmitter("assets/games/Tanks/particles/miniexplosion.json");
					emitter->position = emitter->prevPosition = 50.0f * bullets[i]->GetPosition();
					emitter->life = 0.15;
					boom->play();

					world->DestroyBody(bullets[i]);
					bullets.erase(bullets.begin() + i);
					erased = true;
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
	glm::mat4 cameraMatrix;
	cameraMatrix = glm::scale(cameraMatrix, glm::vec3(0.5, 0.5, 0.5));
	cameraMatrix *= settings->scaleMatrix;
	cameraMatrix = glm::translate(cameraMatrix, glm::vec3(1920, 1080,0));


	spriteBatch->begin(cameraMatrix);
	spriteBatch->draw(back, blib::math::easyMatrix(back, blib::math::Rectangle(-1920,-1080,1920*2, 1080*2)));

	

//	spriteBatch->draw(tankSprite->getFrame(0, 0), blib::math::easyMatrix(glm::vec2(0, 0)), tankSprite->getFrame(0, 0)->center + glm::vec2(0, 0));// , blib::math::Rectangle(0, 0, 1, 1), glm::vec4(1, 1, 1, 1));


	for (auto p : players)
	{
		{
			int frame = ((int)round((glm::degrees(p->body->GetAngle())) / 360 * 64)-1) % 64;
			while (frame < 0)
				frame += 64;

			int x = frame % 8;
			int y = frame / 8;
			spriteBatch->draw(tankSprite->getFrame(x, y), p->getMatrix(settings), tankSprite->getFrame(x, y)->center + glm::vec2(0,0), blib::math::Rectangle(0, 0, 1, 1), p->alive ? p->participant->color : blib::Color::grey);
		}
		{
			int frame = ((int)round((glm::degrees(p->turret->GetAngle())) / 360 * 64)-1) % 64;
			while (frame < 0)
				frame += 64;

			int x = frame % 8;
			int y = frame / 8;

			spriteBatch->draw(turretSprite->getFrame(x, y), p->getMatrix(settings), turretSprite->getFrame(x, y)->center + glm::vec2(0,0), blib::math::Rectangle(0, 0, 1, 1), p->alive ? p->participant->color : blib::Color::grey);

		}
	}

	for(auto b : bullets)
	{
		glm::mat4 matrix;
		matrix = glm::translate(matrix, glm::vec3(100.0f*b->GetPosition().x, 100.0f*b->GetPosition().y, 0));


		matrix = glm::rotate(matrix, glm::degrees(glm::atan(b->GetLinearVelocity().y, b->GetLinearVelocity().x))+90, glm::vec3(0, 0, 1));

		spriteBatch->draw(bulletSprite, matrix, bulletSprite->center);
	}
	for(auto c : crates)
	{
		glm::mat4 matrix;

		float scaleFac = 1;

		matrix = glm::translate(matrix, glm::vec3(100.0f*c->GetPosition().x, 100.0f*c->GetPosition().y, 0));
		//matrix = glm::rotate(matrix, glm::degrees(c->GetAngle()), glm::vec3(0, 0, 1));
		matrix = glm::scale(matrix, glm::vec3(scaleFac, scaleFac, 0));

		spriteBatch->draw(crateSprite, matrix, crateSprite->center);
	}

	for (int ii = 0; ii < 34; ii++)
	{
		for (int i = 0; i < 60; i++)
		{
			if(level[ii*60+i] == 'x')
				spriteBatch->draw(wallSprite, blib::math::easyMatrix(glm::vec2((i-30) * 64, (ii-17) * 64)));
		}
	}



	spriteBatch->end();

	particleSystem->renderState.activeFbo = spriteBatch->renderState.activeFbo;
	particleSystem->draw(glm::scale(cameraMatrix, glm::vec3(2,2,1)));

	glm::mat4 matrix = settings->scaleMatrix;
	matrix = glm::translate(matrix, glm::vec3(settings->center, 0));
	matrix = glm::scale(matrix, glm::vec3(50.0f, 50.0f, 1));

	lineBatch->begin(matrix);
//	world->DrawDebugData();
	lineBatch->end();
}

blib::Texture* Tanks::getTitleImage()
{
	return NULL;
}
