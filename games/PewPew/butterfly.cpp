#include "butterfly.h"
#include "collisionlayers.h"

#include <Box2D/Box2D.h>
#include <blib/Animation.h>
#include <blib/Math.h>
#include <blib/Util.h>

#define _USE_MATH_DEFINES
#include <math.h>

Butterfly::Butterfly(b2World* world, blib::ResourceManager* resourceManager, Player* player)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;

	bodyDef.linearDamping = 0.75f;
	bodyDef.angularVelocity = 0;
	bodyDef.allowSleep = false;
	bodyDef.gravityScale = 0;
	body = world->CreateBody(&bodyDef);

	b2CircleShape* shape = new b2CircleShape();
	shape->m_radius = 0.05f;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = shape;
	fixtureDef.filter.categoryBits = COL_BUTTERFLY;
	fixtureDef.filter.maskBits = COL_LEVEL;

	fixtureDef.density = 0.001f;//non-0 = static
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;
	body->CreateFixture(&fixtureDef);

	int color = rand()%3;
	if(color == 0)
		animation = new blib::Animation("assets/textures/enemies/butterfly_blue.png.json", resourceManager);
	else if(color == 1)
		animation = new blib::Animation("assets/textures/enemies/butterfly_orange.png.json", resourceManager);
	else
		animation = new blib::Animation("assets/textures/enemies/butterfly_yellow.png.json", resourceManager);

	animation->setState("fly");
	animation->update(blib::math::randomDouble(0,10));
	currentState = new FlyState();

}

void Butterfly::init( Level* level, b2World* world )
{
	body->SetTransform((position + size*glm::vec2(0.5f, 0.5f)) / 128.0f, 0);


}



Butterfly::FlyState::FlyState()
{
	rotation = blib::math::randomFloat(0, 2*blib::math::pif);
	rotinc = 0;
	time = 0;
}


Enemy::State* Butterfly::FlyState::update(Enemy* enemy, double elapsedTime)
{
	/*glm::vec2 diff = enemy->body->GetPosition() - enemy->player->body->GetPosition();
	if(glm::length(diff) < 0.75)
	{
		rotation = atan2(diff.y, diff.x);
	}

	enemy->body->ApplyForceToCenter(blib::math::randomFloat(0, 0.00001f) * blib::util::fromAngle(rotation));

	rotation += rotinc * elapsedTime;
	rotinc += blib::math::randomFloat(-1, 1) * elapsedTime;
	rotinc = glm::clamp(rotinc, -5.0f, 5.0f);

	time += elapsedTime;
	if(time > 0.5)
	{
		rotinc = blib::math::randomFloat(-5.0f, 5.0f);
		time = 0;
	}
	*/
	return NULL;
}


Butterfly::IdleState::IdleState()
{

}


Enemy::State* Butterfly::IdleState::update(Enemy* enemy, double elapsedTime)
{
	return NULL;
}