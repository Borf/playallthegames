#pragma once

#include "Enemy.h"

namespace bugswat
{
	class SwirlEnemy : public Enemy
	{
		float bla;
	public:
		SwirlEnemy();
		void update(float elapsedTime);
	};
}