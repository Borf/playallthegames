#include "Zombie.h"


Zombie::Zombie()
{
	rotation = 0;
	timer = 0;
}


Zombie::~Zombie()
{
	delete zombieSprite;
}
