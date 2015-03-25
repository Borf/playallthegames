#include "BasicEnemy.h"

namespace jumpman
{
	void BasicEnemy::update(float speed)
	{
		position.x -= 13 * speed;
	}

}