#pragma once

#include "Enemy.h"

namespace bugswat
{
	class BugSwat;

	class DodgeEnemy : public Enemy
	{
		BugSwat* bugSwat;
	public:
		DodgeEnemy(BugSwat* bugSwat);
		void update(float elapsedTime);
	};
}