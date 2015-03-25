#pragma once

#include "Enemy.h"

namespace jumpman
{
	class BasicEnemy : public Enemy
	{
	public:
		virtual void update(float speed);

	};
}