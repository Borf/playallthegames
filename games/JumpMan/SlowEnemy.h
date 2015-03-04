#pragma once

#include "Enemy.h"

class SlowEnemy : public Enemy
{
public:
	virtual void update( float speed );

};