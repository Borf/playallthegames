#include "Zombie.h"
#include <blib/Animation.h>

Zombie::Zombie()
{
	rotation = 0;
	timer = 0;
}


Zombie::~Zombie()
{
	delete zombieSprite;
}
