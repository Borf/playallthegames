#include "SwirlEnemy.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace bugswat
{
	SwirlEnemy::SwirlEnemy()
	{
		bla = 0;
	}

	void SwirlEnemy::update(float elapsedTime)
	{
		Enemy::update(elapsedTime);
		if (alive)
		{
			if (bla > 7 && bla < 7 + 2 * M_PI)
				rotation += 0.1f*elapsedTime;
			bla += 0.1f * elapsedTime;
		}
	}

}