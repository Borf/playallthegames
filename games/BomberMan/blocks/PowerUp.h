#pragma once

#include "Block.h"

class PowerUp : public Block
{
public:
	PowerUp(glm::vec2 position, int blockSize);

	virtual bool canExplode();
	virtual bool blocks();
	virtual bool onTouch(BomberManPlayer* player) = 0;
};