#pragma once

#include "Enemy.h"

class SwirlEnemy : public Enemy
{
	float bla;
public:
	SwirlEnemy();
	void update(float elapsedTime);
};