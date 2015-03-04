#include "Wheel.h"
#include <Box2D/Box2D.h>
#include "MacroMachinesPlayer.h"

Wheel::Wheel(b2World* world, MacroMachinesPlayer* player, const glm::vec2 &position, bool powered, float revolvePower)
{
	this->player = player;
	this->powered = powered;
	this->revolvePower = revolvePower;

	pos = position;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.linearDamping = 0.15f;
	bodyDef.angularDamping = 0.3f;
	bodyDef.position = player->body->GetWorldPoint(position);
	bodyDef.angle = player->body->GetAngle();
	body = world->CreateBody(&bodyDef);

	b2PolygonShape shape;
	shape.SetAsBox(0.1f, 0.4f);

	b2FixtureDef fixtureDef;
	fixtureDef.density = 10;
	fixtureDef.friction = 0.3f;
	fixtureDef.shape = &shape;
	fixtureDef.isSensor = true;
	body->CreateFixture(&fixtureDef);

	//create joint to connect wheel to body

	b2Joint* joint;

	if (this->revolvePower != 0)
	{
		b2RevoluteJointDef jointDef;
		jointDef.Initialize(body, player->body, body->GetWorldCenter());
		jointDef.localAnchorA = body->GetLocalCenter();
		jointDef.localAnchorB = player->body->GetLocalPoint(body->GetWorldCenter());
		joint = world->CreateJoint(&jointDef);
		((b2RevoluteJoint*)joint)->EnableMotor(true);
	}
	else
	{
		b2PrismaticJointDef jointDef;
		jointDef.Initialize(body, player->body, body->GetWorldCenter(), glm::vec2(1, 0));
		joint = world->CreateJoint(&jointDef);
		((b2PrismaticJoint*)joint)->EnableLimit(true);
	}
}

glm::vec2 Wheel::getLocalVelocity()
{
	return player->body->GetLocalVector(player->body->GetLinearVelocityFromLocalPoint(pos));
}

glm::vec2 Wheel::getDirectionVector()
{
	/*
		returns a world unit vector pointing in the direction this wheel is moving
		*/
	glm::vec2 localVelocity = getLocalVelocity();

	glm::vec2 v = (localVelocity.y > 0) ? glm::vec2(0, 1) : glm::vec2(0, -1);

	double angle = atan2(v.y, v.x);
	float len = glm::length(v);
	return len * glm::vec2(glm::cos(angle + this->body->GetAngle()), glm::sin(angle + this->body->GetAngle()));
}

glm::vec2 Wheel::getKillVelocityVector()
{
	/*
		substracts sideways velocity from this wheel's velocity vector and returns the remaining front-facing velocity vector
		*/
	glm::vec2 velocity = body->GetLinearVelocity();
	glm::vec2 sideways_axis = getDirectionVector();

	float dotprod = glm::dot(velocity, sideways_axis);
	return sideways_axis * dotprod;
}

void Wheel::killSidewaysVelocity()
{
	glm::vec2 kv = getKillVelocityVector();
	body->SetLinearVelocity(kv);
}

void Wheel::DampSidewaysVelocity(float elapsedTime)
{
	float fac = 0.0f;
	glm::vec2 kv = getKillVelocityVector();
	body->SetLinearVelocity(fac * body->GetLinearVelocity() + (1 - fac) * kv);
}

void Wheel::setDensity(float density)
{
	b2Fixture* fixture = body->GetFixtureList();
	while (fixture)
	{
		fixture->SetDensity(density);
		fixture = fixture->GetNext();
	}
	body->ResetMassData();
}