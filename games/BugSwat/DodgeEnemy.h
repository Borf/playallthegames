#pragma once

#include "Enemy.h"

class BugSwat;

class DodgeEnemy : public Enemy
{
	BugSwat* bugSwat;
public:
	DodgeEnemy(BugSwat* bugSwat);
	void update(float elapsedTime);
};