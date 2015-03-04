#pragma once

#include "enemy.h"

class Player;
namespace blib { class ResourceManager; };

class Carni : public Enemy
{
public:
	Carni(b2World* world, blib::ResourceManager* resourceManager, Player* player);
	virtual void init(Level* level, b2World* world);


	class IdleState : public Enemy::IdleState
	{
	public:
		virtual State* update(Enemy* enemy, double elapsedtime);
	};

	class AttackState : public Enemy::State
	{
		double stateTime;
	public:
		AttackState()
		{
			stateTime = 0;
		}
		virtual State* update(Enemy* enemy, double elapsedTime);
	};


};