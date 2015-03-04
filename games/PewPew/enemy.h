#pragma once

#include "levelobject.h"

class Player;

class Enemy : public LevelObject<AnimationGameObject>
{
protected:
	class State
	{
	public:
		virtual State* update(Enemy* enemy, double elapsedTime) = 0;

	};

	class IdleState : public State
	{
	public:
		virtual State* update(Enemy* enemy, double elapsedTime);
	};

	class PatrolState : public State
	{
	public:
		virtual State* update(Enemy* enemy, double elapsedTime);
	};

	State* currentState;


public:
	Enemy();

	Player* player;
	virtual void update(double elapsedTime);
};