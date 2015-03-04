#pragma once

#include "Enemy.h"

class JumpEnemy : public Enemy
{
	float jumpSpeed;
public:
	JumpEnemy();
	virtual void update(float speed);

};