#include "PowerUp.h"


PowerUp::PowerUp( glm::vec2 position, int blockSize ) : Block(position, blockSize)
{
}

bool PowerUp::canExplode()
{
	return true;
}

bool PowerUp::blocks()
{
	return false;
}



