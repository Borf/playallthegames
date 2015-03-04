#include "chargerbird.h"

#include <Box2D/Box2D.h>
#include <blib/Animation.h>
#include "collisionlayers.h"

class Player;

ChargerBird::ChargerBird(b2World* world, blib::ResourceManager* resourceManager, Player* player)
{
	this->player = player;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;

	bodyDef.linearDamping = 0.75f;
	bodyDef.angularVelocity = 0;
	bodyDef.allowSleep = false;
	bodyDef.gravityScale = 0;
	body = world->CreateBody(&bodyDef);

	b2CircleShape* shape = new b2CircleShape();
	shape->m_radius = 0.25f;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = shape;
	fixtureDef.filter.categoryBits = COL_ENEMIES;
	fixtureDef.filter.maskBits = COL_ALL - COL_BUTTERFLY - COL_SCARF;

	fixtureDef.density = 10.0f;//non-0 = static
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.0f;
	body->CreateFixture(&fixtureDef);


	animation = new blib::Animation("assets/textures/enemies/bird.png.json", resourceManager);
	animation->setState("fly");
	currentState = new IdleState();

}

void ChargerBird::init( Level* level, b2World* world )
{
	body->SetTransform((position + size*glm::vec2(0.5f, 0.5f)) / 128.0f, 0);


}



Enemy::State* ChargerBird::IdleState::update( Enemy* enemy, double elapsedtime )
{
	ChargerBird* bird = static_cast<ChargerBird*>(enemy);
	/*

	glm::vec2 playerPosition = bird->player->body->GetPosition();
	glm::vec2 birdPosition = bird->body->GetPosition();
	
	if(glm::length(playerPosition - birdPosition) < 3)
	{
		bird->animation->setState("alerted");
		return new AlertedState();
	}*/
	return NULL;
}

Enemy::State* ChargerBird::AlertedState::update( Enemy* enemy, double elapsedTime )
{
	/*ChargerBird* bird = static_cast<ChargerBird*>(enemy);
	stateTime += elapsedTime;
	if(stateTime > 0.7)
	{
		glm::vec2 playerPosition = bird->player->body->GetPosition();
		glm::vec2 birdPosition = bird->body->GetPosition();
		glm::vec2 diff = birdPosition - playerPosition;
		bird->body->SetTransform(bird->body->GetPosition(), atan2(diff.y, diff.x));

		float angle = bird->body->GetAngle();
		bird->body->ApplyForceToCenter(10 * b2Vec2(-cos(angle), -sin(angle)));

	}

	if(stateTime > 1.5)
	{
		glm::vec2 playerPosition = bird->player->body->GetPosition();
		glm::vec2 birdPosition = bird->body->GetPosition();
		glm::vec2 diff = birdPosition - playerPosition;
		bird->body->SetTransform(bird->body->GetPosition(), atan2(diff.y, diff.x));

		bird->animation->setState("fly");
		return new ChaseState();
	}*/
	return NULL;
}

Enemy::State* ChargerBird::ChaseState::update( Enemy* enemy, double elapsedTime )
{
	/*ChargerBird* bird = static_cast<ChargerBird*>(enemy);

	float angle = bird->body->GetAngle();
	bird->body->ApplyForceToCenter(40 * b2Vec2(-cos(angle), -sin(angle)));


	b2ContactEdge* contactEdge = bird->body->GetContactList();
	while(contactEdge)
	{
		b2Body* body1 = contactEdge->contact->GetFixtureA()->GetBody();
		b2Body* body2 = contactEdge->contact->GetFixtureB()->GetBody();

		bird->animation->setState("headbutt");
		return new StunnedState();


		contactEdge = contactEdge->next; 
	}
	*/

	return NULL;
}


Enemy::State* ChargerBird::StunnedState::update(Enemy* enemy, double elapsedTime)
{
	stateTime += elapsedTime;
	enemy->body->SetLinearDamping(20);
	if(stateTime > 0.7)
	{
		enemy->body->SetLinearDamping(0);
		enemy->animation->setState("fly");
		return new IdleState();
	}
	return NULL;
}