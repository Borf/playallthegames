#include "JumpEnemy.h"

namespace jumpman
{

	JumpEnemy::JumpEnemy()
	{
		jumpSpeed = 0;
	}

	void JumpEnemy::update(float speed)
	{
		position.x -= 13 * speed;
		position.y += jumpSpeed*speed;
		jumpSpeed += 0.8f*speed;
		if (position.y > 760)
			position.y = 760;

		if (position.x > 400 && position.x < 500 && position.y == 760)
			jumpSpeed = -30;
	}

}