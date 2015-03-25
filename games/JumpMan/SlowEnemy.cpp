#include "SlowEnemy.h"
namespace jumpman
{
	void SlowEnemy::update(float speed)
	{
		if (position.x < 500 && position.x > 300)
			position.x -= 5 * speed;
		else
			position.x -= 13 * speed;
	}

}