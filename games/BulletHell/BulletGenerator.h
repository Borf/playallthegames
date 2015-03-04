#pragma once

class Bullet;

class BulletGenerator
{
public:
	virtual Bullet* getBullet(float currentTime, float speed) = 0;
};

