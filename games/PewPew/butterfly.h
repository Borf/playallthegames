#pragma once

#include "enemy.h"

namespace blib { class ResourceManager; };
class b2World;
class Player;


class Butterfly : public Enemy
{
public:
	Butterfly(b2World* world, blib::ResourceManager* resourceManager, Player* player);
	virtual void init(Level* level, b2World* world);


	class FlyState : public Enemy::State
	{
		double time;
		float rotation;
		float rotinc;
	public:
		FlyState();
		State* update(Enemy* enemy, double elapsedTime);
	};


	class IdleState : public Enemy::State
	{
	public:
		IdleState();
		State* update(Enemy* enemy, double elapsedTime);
	};

};