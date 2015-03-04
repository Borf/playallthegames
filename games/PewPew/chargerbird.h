#pragma once

#include "enemy.h"

namespace blib { class ResourceManager; };
class b2World;
class Player;

class ChargerBird : public Enemy
{
public:
	ChargerBird(b2World* world, blib::ResourceManager* resourceManager, Player* player);
	virtual void init(Level* level, b2World* world);


	class IdleState : public Enemy::IdleState
	{
	public:
		virtual State* update(Enemy* enemy, double elapsedtime);
	};

	class AlertedState : public Enemy::State
	{
		double stateTime;
	public:
		AlertedState()
		{
			stateTime = 0;
		}
		virtual State* update(Enemy* enemy, double elapsedTime);
	};


	class ChaseState : public Enemy::State
	{
	public:
		virtual State* update(Enemy* enemy, double elapsedTime);
	};

	class StunnedState : public Enemy::State
	{
		double stateTime;
	public:
		StunnedState()
		{
			stateTime = 0;
		}
		virtual State* update(Enemy* enemy, double elapsedTime);
	};

};