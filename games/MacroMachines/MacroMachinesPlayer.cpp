#include "MacroMachinesPlayer.h"
#include "Wheel.h"

#include <blib/linq.h>
#include <blib/util/Log.h>
#include <Box2D/Box2D.h>

using blib::util::Log;

MacroMachinesPlayer::MacroMachinesPlayer(int index) : ScorePlayer(index)
{

}

void MacroMachinesPlayer::spawn(b2World* world, glm::vec2 position, Drive drive, float rotation)
{
	levelIndex = 0;
	score = 0;
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = position;
	bodyDef.angle = rotation;
	bodyDef.linearDamping = 0.15f;
	bodyDef.angularDamping = 0.3f;
	bodyDef.bullet = true;
	bodyDef.position = position;
	body = world->CreateBody(&bodyDef);

	b2PolygonShape shape;
	shape.SetAsBox(Width, Height);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1;
	fixtureDef.restitution = 0.2f;
	fixtureDef.friction = 0.8f;
	body->CreateFixture(&fixtureDef);

	wheels.push_back(new Wheel(world, this, glm::vec2(-0.95f, -1.15f), drive == Drive::FWD || drive == Drive::AWD, 1));
	wheels.push_back(new Wheel(world, this, glm::vec2(0.95f, -1.15f), drive == Drive::FWD || drive == Drive::AWD, 1));
	wheels.push_back(new Wheel(world, this, glm::vec2(-0.95f, 1.15f), drive == Drive::RWD || drive == Drive::AWD, 0));
	wheels.push_back(new Wheel(world, this, glm::vec2(0.95f, 1.15f), drive == Drive::RWD || drive == Drive::AWD, 0));
}

float MacroMachinesPlayer::getSpeed()
{
	return body->GetLinearVelocity().Length();
}

glm::vec2 MacroMachinesPlayer::getLocalVelocity()
{
	return body->GetLocalVector(body->GetLinearVelocityFromLocalPoint(glm::vec2(0, 0)));
}

void MacroMachinesPlayer::update(float elapsedTime)
{
	for (Wheel* w : wheels)
		if (w->revolvePower == 0)
			w->killSidewaysVelocity();
	for (Wheel* w : wheels)
		if (w->revolvePower != 0)
			w->DampSidewaysVelocity(elapsedTime);

	for (Wheel* w : wheels)
		if (w->revolvePower != 0)
			w->body->SetTransform(w->body->GetPosition(), body->GetAngle() + w->revolvePower * steering);






	//3. APPLY FORCE TO WHEELS

	//if accelerator is pressed down and speed limit has not been reached, go forwards
	if ((forward) && (getSpeed() < max_speed))
	{
		for(auto w : wheels)
		{
			if (w->powered)
			{
				glm::vec2 position = w->body->GetWorldCenter();
				glm::vec2 force = w->body->GetWorldVector(glm::vec2(0, -power));
				w->body->ApplyForce(force, position);
			}
		}
		if (!braking)
		{
			for(auto w : wheels)
			{
				w->setDensity(2.0f);
			}
		}
	}
	if (braking)
	{
		if (getLocalVelocity().y < 0)
		{
			for(auto w : wheels)
			{
				if (!forward)
					w->body->ApplyForceToCenter(w->body->GetWorldVector(glm::vec2(0, 1.3f * power)));
				w->setDensity(w->revolvePower == 0 ? 1.7f : 0.4f);
			}
		}
		else
		{//reverse
			if (getSpeed() < max_speed * 0.25f)
			{
				for(auto w : wheels)
				{
					if (w->powered)
						w->body->ApplyForceToCenter(w->body->GetWorldVector(glm::vec2(0, 0.25f * power)));
				}
			}
		}
	}
}
