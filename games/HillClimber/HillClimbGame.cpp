#include "HillClimbGame.h"

#include "../../playallthegames/Participant.h"

#include <blib/LineBatch.h>
#include <blib/SpriteBatch.h>
#include <blib/Color.h>
#include <blib/Font.h>
#include <blib/util/stb_perlin.h>
#include <blib/Box2DDebug.h>
#include <blib/util/Log.h>
#include <blib/Color.h>
#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/math.h>
#include <glm/gtc/matrix_transform.hpp>
using blib::util::Log;

namespace hillclimb
{
	HillClimb::HillClimb()
	{
		world = NULL;
		controls[ControllerButton::ButtonA] = "Forward";
		controls[ControllerButton::ButtonB] = "Backward";
		controls[ControllerButton::ButtonX] = "Boost";
		controls[ControllerButton::TriggersLeft] = "Flip CCW";
		controls[ControllerButton::TriggersRight] = "Flip CW";
	}
	HillClimb::~HillClimb()
	{
		if (world)
			delete world;
		world = NULL;
	}


	std::string HillClimb::getName()
	{
		return "HillClimb";
	}

	std::string HillClimb::getInstructions()
	{
		return "Get Far, Don't crash";
	}

	std::pair<int, int> HillClimb::getPlayerCount()
	{
		return std::pair<int, int>(99, 99);
	}

	void HillClimb::loadResources()
	{
		whitePixel = resourceManager->getResource<blib::Texture>("assets/textures/whitepixel.png");
		tireSprite = resourceManager->getResource<blib::Texture>("assets/games/HillClimb/tire.png");
		grassSprite = resourceManager->getResource<blib::Texture>("assets/games/HillClimb/grass.png");
		skySprite = resourceManager->getResource<blib::Texture>("assets/games/HillClimb/sky.png");
		carSprite = resourceManager->getResource<blib::Texture>("assets/games/HillClimb/car.png");
		carOverlaySprite = resourceManager->getResource<blib::Texture>("assets/games/HillClimb/carOverlay.png");
		shadowSprite = resourceManager->getResource<blib::Texture>("assets/games/HillClimb/shadow.png");
		font = resourceManager->getResource<blib::Font>("tahoma");
		backgroundColor = blib::Color::black;
	}




	float level = 0;


	float noiseFunc(float f)
	{
		return
			10 * cos(f / 3000) * stb_perlin_noise3(f / (300.0f / (f / 3000000.0f)), 0, level) +
			0.1f * stb_perlin_noise3(f / 300.0f, 0.5f, 0.5f + level) +


			0;
	}


	void HillClimb::start()
	{
		if (world)
			delete world;
		world = new b2World(b2Vec2(0, 9.8f));
		world->SetDebugDraw(blib::Box2DDebug::getInstance());
		blib::Box2DDebug::getInstance()->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit);

		level = rand();
		floorCounter = -1;

		floorBody[0] = NULL;
		floorBody[1] = NULL;

		buildFloor();
		buildFloor();

		for(auto p : players)
		{
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position = b2Vec2(0, -4.4f);
			bodyDef.angularDamping = 5.9f;
			bodyDef.linearDamping = 0.1f;
			b2Body* carBase = world->CreateBody(&bodyDef);

			b2PolygonShape baseShape;
			baseShape.SetAsBox(1.3f, 0.4f);
			b2FixtureDef baseFixture;
			baseFixture.shape = &baseShape;
			baseFixture.density = 5.0f;
			baseFixture.filter.categoryBits = 2;
			baseFixture.filter.maskBits = 1;
			carBase->CreateFixture(&baseFixture);
			carBase->SetSleepingAllowed(false);





			b2CircleShape wheelShape;
			wheelShape.m_radius = 0.5f;
			b2FixtureDef wheelFixture;
			wheelFixture.shape = &wheelShape;
			wheelFixture.friction = 0.5f;
			wheelFixture.density = 5.0f;
			wheelFixture.restitution = 0.35f;
			wheelFixture.filter.categoryBits = 2;
			wheelFixture.filter.maskBits = 1;

			b2BodyDef wheelDef;
			wheelDef.type = b2_dynamicBody;
			wheelDef.position = b2Vec2(1.0f, -4.0f);
			wheelDef.angularDamping = 0.9f;
			b2Body* wheel = world->CreateBody(&wheelDef);
			wheel->CreateFixture(&wheelFixture);

			b2RevoluteJointDef frontMotorDef;
			frontMotorDef.Initialize(carBase, wheel, wheel->GetPosition());
			frontMotorDef.enableMotor = true;
			frontMotorDef.maxMotorTorque = 1000000;
			frontMotorDef.motorSpeed = 0;
			p->backEngine = (b2RevoluteJoint*)world->CreateJoint(&frontMotorDef);
			p->backWheel = wheel;

			wheelDef.position = b2Vec2(-1.0f, -4.0f);
			wheel = world->CreateBody(&wheelDef);
			wheel->CreateFixture(&wheelFixture);
			frontMotorDef.Initialize(carBase, wheel, wheel->GetPosition());
			frontMotorDef.enableMotor = true;
			frontMotorDef.maxMotorTorque = 1000000;
			frontMotorDef.motorSpeed = 0;
			p->frontEngine = (b2RevoluteJoint*)world->CreateJoint(&frontMotorDef);
			p->frontWheel = wheel;


			p->car = carBase;

		}


	}
	
	void HillClimb::update(float elapsedTime)
	{
		world->Step(elapsedTime, 10, 10);
		

		int firstPlayer = blib::linq::max<float, int>(players, [](HillClimbPlayer* p) { return p->car->GetPosition().x; }, [](HillClimbPlayer* p) { return p->index; });

		if (players[firstPlayer]->car->GetPosition().x > (floorCounter - 1) * 100 + 50)
			buildFloor();


		for (auto p : players)
		{
			if (p->lastX < p->car->GetPosition().x)
			{
				p->lastX = p->car->GetPosition().x;
				p->lastTime = gameTime;
				p->alive = true;
			}
			if (p->lastTime + 7 < gameTime)
				p->alive = false;
			if (p->car->GetPosition().x < players[firstPlayer]->car->GetPosition().x - 40)
				p->alive = false;


			if (p->joystick.a != 0)
			{
				p->frontWheel->SetAngularDamping(0.0f);
				p->backWheel->SetAngularDamping(0.0f);
				if (!p->frontEngine->IsMotorEnabled())
				{
					p->frontEngine->EnableMotor(true);
					p->backEngine->EnableMotor(true);
					p->frontEngine->SetMotorSpeed(p->frontWheel->GetAngularVelocity());
					p->backEngine->SetMotorSpeed(p->backWheel->GetAngularVelocity());
				}
				p->frontEngine->SetMotorSpeed(glm::min(p->frontEngine->GetMotorSpeed() + elapsedTime * 30, p->maxSpeed));
				p->backEngine->SetMotorSpeed(glm::min(p->frontEngine->GetMotorSpeed() + elapsedTime * 30, p->maxSpeed));
			}
			else if (p->joystick.b != 0)
			{
				p->frontWheel->SetAngularDamping(0.0f);
				p->backWheel->SetAngularDamping(0.0f);
				if (!p->frontEngine->IsMotorEnabled())
				{
					p->frontEngine->EnableMotor(true);
					p->backEngine->EnableMotor(true);
					p->frontEngine->SetMotorSpeed(p->frontWheel->GetAngularVelocity());
					p->backEngine->SetMotorSpeed(p->backWheel->GetAngularVelocity());
				}
				p->frontEngine->SetMotorSpeed(glm::max(p->frontEngine->GetMotorSpeed() - elapsedTime * 30, -p->maxSpeed));
				p->backEngine->SetMotorSpeed(glm::max(p->frontEngine->GetMotorSpeed() - elapsedTime * 30, -p->maxSpeed));
			}
			else
			{
				p->frontEngine->EnableMotor(false);
				p->backEngine->EnableMotor(false);
				p->frontWheel->SetAngularDamping(0.5f);
				p->backWheel->SetAngularDamping(0.5f);
			}

			if (p->joystick.x == 1 && p->prevJoystick.x != 1)
			{
				p->car->ApplyLinearImpulse(blib::math::fromAngle(p->car->GetAngle()) * 100.0f * (float) p->loops, p->car->GetWorldCenter(), true);
				p->loops = 0;
			}

			if (!p->frontWheel->GetContactList() && !p->backWheel->GetContactList())
			{
				float diff = p->lastAngle - p->car->GetAngle();
				if (diff < -blib::math::pif)
					diff += 2 * blib::math::pif;
				if (diff > blib::math::pif)
					diff -= 2 * blib::math::pif;

				p->lastAngle = p->car->GetAngle();
				p->rotations += diff;
			}
			else
			{
				p->lastAngle = p->car->GetAngle();
				if (fabs(p->rotations) > 0.1f)
				{
					p->loops += glm::round(glm::abs(p->rotations) / blib::math::pif);
				}
				p->rotations = 0;
			}

			p->car->ApplyTorque(200 * (p->joystick.rightTrigger - p->joystick.leftTrigger), true);

		}

	}


	void HillClimb::draw()
	{
		int firstPlayer = blib::linq::max<float, int>(players, [](HillClimbPlayer* p) { return p->car->GetPosition().x; }, [](HillClimbPlayer* p) { return p->index; });
		float center = players[firstPlayer]->car->GetPosition().x;
		if (!hasWinner())
		{
			int lastPlayer = blib::linq::min<float, int>(blib::linq::where(players, [](HillClimbPlayer* p) { return p->alive; }), [](HillClimbPlayer* p) { return p->car->GetPosition().x; }, [](HillClimbPlayer* p) { return p->index; });
			center = (players[firstPlayer]->car->GetPosition().x + players[lastPlayer]->car->GetPosition().x) / 2.0f;
		}


		glm::mat4 transform;
		transform = glm::translate(transform, glm::vec3(1920/2, 1080/2, 0.0f));
		transform = glm::scale(transform, glm::vec3(50, 50, 1.0f));
		transform = glm::translate(transform, glm::vec3(-center, 2, 0.0f));




		lineBatch->begin(transform);

		int pos = (int)(center*2);


		spriteBatch->begin(transform);


		std::vector<std::tuple<glm::vec2, glm::vec2, glm::vec4>> verts;


		glm::vec4 prevFac(0, 0, 0, -1);
		for (int i = pos - 40; i < pos + 40; i++)
		{
			float x1 = i / 2.0f;
			float h1 = noiseFunc(i * 100);

			float x2 = (i+1) / 2.0f;
			float h2 = noiseFunc((i+1) * 100);

			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x1, h1), glm::vec2(), glm::vec4(0.608, 0.478, 0.004, 1)));
			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x2, h2), glm::vec2(), glm::vec4(0.608, 0.478, 0.004, 1)));
			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x1, 5), glm::vec2(), glm::vec4(0.608, 0.478, 0.004, 1)));

			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x2, h2), glm::vec2(), glm::vec4(0.608, 0.478, 0.004, 1)));
			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x2, 5), glm::vec2(), glm::vec4(0.608, 0.478, 0.004, 1)));
			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x1, 5), glm::vec2(), glm::vec4(0.608, 0.478, 0.004, 1)));


			glm::vec2 normal = glm::normalize(glm::vec2(x1, h1) - glm::vec2(x2, h2));
			glm::vec4 fac = glm::vec4(1,1,1,1) * glm::pow(glm::abs(glm::dot(normal, glm::normalize(glm::vec2(-1, 1)))), 1.5f);
			fac.a = 1;

			if (prevFac.a == -1)
				prevFac = fac;

			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x1, h1), glm::vec2(), prevFac * glm::vec4(0.608, 0.478, 0.004, 1)));
			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x2, h2), glm::vec2(), fac * glm::vec4(0.608, 0.478, 0.004, 1)));
			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x1, h1-6), glm::vec2(), prevFac * glm::vec4(0.608, 0.478, 0.004, 1)));

			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x2, h2), glm::vec2(), fac * glm::vec4(0.608, 0.478, 0.004, 1)));
			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x2, h2-6), glm::vec2(), fac * glm::vec4(0.608, 0.478, 0.004, 1)));
			verts.push_back(std::make_tuple<glm::vec2, glm::vec2, glm::vec4>(glm::vec2(x1, h1-6), glm::vec2(), prevFac * glm::vec4(0.608, 0.478, 0.004, 1)));
			prevFac = fac;
		}


		spriteBatch->draw(skySprite, blib::math::easyMatrix(glm::vec2(pos/2.0f-40, -30), 0, glm::vec2(100, 0.06f)));

		for (int i = ((pos - 60)/16)*16; i < pos + 40; i+=16)
			spriteBatch->draw(grassSprite, blib::math::easyMatrix(glm::vec2(i/2, 5), 0, 0.075f));
	
		spriteBatch->draw(whitePixel, glm::mat4(), verts);
		


		std::vector<HillClimbPlayer*> sorted = players;
		std::sort(sorted.begin(), sorted.end(), [](HillClimbPlayer* a, HillClimbPlayer* b)
		{
			return a->index > b->index;
		});

		for (auto p : sorted)
		{
			float playerHeight = -(0.5 + (5 / players.size()) * p->index);

			float floorHeight = noiseFunc(p->car->GetPosition().x * 100 * 2);
			float floorHeight2 = noiseFunc((p->car->GetPosition().x + 1) * 100 * 2);
			glm::vec2 diff(-1, floorHeight - floorHeight2);
			float normal = atan2(diff.y, diff.x);
			

			spriteBatch->draw(shadowSprite, blib::math::easyMatrix(glm::vec2(p->car->GetPosition().x, floorHeight + playerHeight), glm::degrees(normal), 0.0045f), shadowSprite->center);

			spriteBatch->draw(carSprite, blib::math::easyMatrix(p->car->GetPosition() + glm::vec2(0, playerHeight), glm::degrees(p->car->GetAngle()), 0.0045f), carSprite->center + glm::vec2(0, 0), p->participant->color);
			spriteBatch->draw(carOverlaySprite, blib::math::easyMatrix(p->car->GetPosition() + glm::vec2(0, playerHeight), glm::degrees(p->car->GetAngle()), 0.0045f), carSprite->center + glm::vec2(0, 0));

			spriteBatch->draw(tireSprite, blib::math::easyMatrix(p->frontWheel->GetPosition() + glm::vec2(0, playerHeight), glm::degrees(p->frontWheel->GetAngle()), 0.0075f), tireSprite->center);
			spriteBatch->draw(tireSprite, blib::math::easyMatrix(p->backWheel->GetPosition() + glm::vec2(0, playerHeight), glm::degrees(p->frontWheel->GetAngle()), 0.0075f), tireSprite->center);
		}


		for (auto p : players)
		{
			float playerHeight = -(0.5 + (5 / players.size()) * p->index);
			spriteBatch->draw(font, std::to_string(p->loops), blib::math::easyMatrix(p->car->GetPosition() + glm::vec2(-0.1f, playerHeight - 0.15f), 0, 0.03f));
		}

		spriteBatch->end();


		lineBatch->begin(transform);
		//world->DrawDebugData();
		lineBatch->end();



	}

	blib::Texture* HillClimb::getTitleImage()
	{
		return NULL;
	}

	void HillClimb::buildFloor()
	{
		b2Vec2 verts[201];
		for (int i = 0; i < 201; i++)
		{
			float h = noiseFunc((i+ 200 * floorCounter) * 100);
			verts[i] = b2Vec2(i / 2.0f, h);
		}


		b2ChainShape floorShape;
		floorShape.CreateChain(verts, 201);

		b2FixtureDef floorFixture;
		floorFixture.shape = &floorShape;
		floorFixture.density = 0;
		floorFixture.friction = 1.0f;
		floorFixture.filter.categoryBits = 1;


		b2BodyDef floorBodyDef;
		floorBodyDef.type = b2_staticBody;
		floorBodyDef.position = b2Vec2(100 * floorCounter, 0);

		if (floorBody[(floorCounter + 2) % 2])
			world->DestroyBody(floorBody[(floorCounter + 2) % 2]);

		floorBody[(floorCounter+2)%2] = world->CreateBody(&floorBodyDef);
		floorBody[(floorCounter + 2) %2]->CreateFixture(&floorFixture);
		floorCounter++;
	}

}