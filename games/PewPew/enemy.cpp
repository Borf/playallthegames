#include "enemy.h"

#include <blib/Animation.h>

Enemy::Enemy()
{
	currentState = NULL;
	flipHorizontal = true;
}


void Enemy::update(double elapsedTime)
{
	animation->update(elapsedTime);
	if(currentState)
	{
		State* newState = currentState->update(this, elapsedTime);
		if(newState != NULL)
		{
			delete currentState;
			currentState = newState;
		}
	}
}

Enemy::State* Enemy::IdleState::update(Enemy* enemy, double elapsedTime)
{
	return NULL;
}