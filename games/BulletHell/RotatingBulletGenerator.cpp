#include "RotatingBulletGenerator.h"
#include "Bullet.h"

#include <blib/Util.h>


RotatingBulletGenerator::RotatingBulletGenerator( glm::vec2 position, float bulletSpawnDelay )
{
	this->position = position;
	this->bulletSpawnDelay = bulletSpawnDelay;
}

Bullet* RotatingBulletGenerator::getBullet( float currentTime, float speed )
{
	if (currentTime - lastBullet > bulletSpawnDelay / speed)
	{
		lastBullet = currentTime;
		return new Bullet(position + glm::vec2(960 * glm::sin(currentTime / 2), 0), 400.0f * blib::util::fromAngle(currentTime* 2));
	}
	return NULL;
}
