#pragma once

#include "Enemy.h"

namespace jumpman
{
	class SlowEnemy : public Enemy
	{
	public:
		virtual void update(float speed);

	};
}