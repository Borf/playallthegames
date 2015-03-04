#include "carni.h"
#include "collisionlayers.h"

#include <blib/Animation.h>

Carni::Carni(b2World* world, blib::ResourceManager* resourceManager, Player* player)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;

	bodyDef.linearDamping = 0.75f;
	bodyDef.angularVelocity = 0;
	bodyDef.allowSleep = false;
	bodyDef.gravityScale = 0;
	body = world->CreateBody(&bodyDef);

	b2CircleShape* shape = new b2CircleShape();
	shape->m_radius = 0.3f;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = shape;
	fixtureDef.filter.categoryBits = COL_ENEMIES;
	fixtureDef.filter.maskBits = COL_ALL - COL_BUTTERFLY - COL_SCARF;

	fixtureDef.density = 10.0f;//non-0 = static
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;
	body->CreateFixture(&fixtureDef);

	animation = new blib::Animation("assets/textures/enemies/carni.png.json", resourceManager);
	animation->setState("idle");
	currentState = new IdleState();

}

void Carni::init( Level* level, b2World* world )
{
	body->SetTransform((position + size*glm::vec2(0.5f, 0.5f)) / 128.0f, 0);


}




Enemy::State* Carni::IdleState::update( Enemy* enemy, double elapsedtime )
{
	/*Carni* carni = static_cast<Carni*>(enemy);

	glm::vec2 playerPosition = carni->player->body->GetPosition();
	glm::vec2 carniPosition = carni->body->GetPosition();

	if(glm::length(playerPosition - carniPosition) < 1)
	{
		carni->animation->setState("attack");
		return new AttackState();
	}*/
	return NULL;
}

Enemy::State* Carni::AttackState::update( Enemy* enemy, double elapsedTime )
{
	Carni* carni = static_cast<Carni*>(enemy);
	stateTime += elapsedTime;

	if(stateTime > 1.0)
	{
		carni->animation->setState("idle");
		return new IdleState();
	}
	return NULL;
}