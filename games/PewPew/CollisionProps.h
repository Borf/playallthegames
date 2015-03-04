#pragma once


class CollisionProperties
{
public:
	enum Type
	{
		BULLET,
		WALL,
		BUTTON,
		GRAVITYOBJECT,
		PLAYER,
		VENT,
		ZAPPER,
	};

	Type type;

	CollisionProperties(Type type)
	{
		this->type = type;
	}
};

class Bullet;
class BulletCollision : public CollisionProperties
{
public:
	Bullet* bullet;
	BulletCollision(Bullet* bullet) : CollisionProperties(BULLET)
	{
		this->bullet = bullet;
	}
};

class LevelObjectBase;
class ButtonCollision : public CollisionProperties
{
public:
	LevelObjectBase* button;
	ButtonCollision(LevelObjectBase* button) : CollisionProperties(BUTTON)
	{
		this->button = button;
	}
};

class GravityObjectCollision : public CollisionProperties
{
public:
	LevelObjectBase* object;
	GravityObjectCollision(LevelObjectBase* object) : CollisionProperties(GRAVITYOBJECT)
	{
		this->object = object;
	}
};



class PlayerCollision : public CollisionProperties
{
public:
	PlayerCollision() : CollisionProperties(PLAYER)
	{
	}
};
class WallCollision : public CollisionProperties
{
public:
	WallCollision() : CollisionProperties(WALL)
	{
	}
};

class VentCollision : public CollisionProperties
{
public:
	VentCollision() : CollisionProperties(VENT)
	{
	}
};

class ZapperCollision : public CollisionProperties
{
public:
	ZapperCollision() : CollisionProperties(ZAPPER)
	{
	}
};
