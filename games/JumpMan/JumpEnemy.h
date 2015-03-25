#pragma once

#include "Enemy.h"
namespace jumpman
{
	class JumpEnemy : public Enemy
	{
		float jumpSpeed;
	public:
		JumpEnemy();
		virtual void update(float speed);

	};
}