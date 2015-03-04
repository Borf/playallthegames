#pragma once

#include "Block.h"


class EmptyBlock : public Block
{
public:
	EmptyBlock(glm::vec2 position, int blockSize);

	virtual void draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet );

};