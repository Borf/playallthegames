#pragma once

#include "BulletGenerator.h"
#include <glm/glm.hpp>

class Bullet;


class RotatingBulletGenerator : public BulletGenerator
{
public:
	glm::vec2 position;
	float lastBullet;
	float bulletSpawnDelay;

	RotatingBulletGenerator(glm::vec2 position, float bulletSpawnDelay);

	Bullet* getBullet(float currentTime, float speed);

};