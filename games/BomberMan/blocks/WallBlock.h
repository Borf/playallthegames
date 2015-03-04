#pragma once

#include "Block.h"


class WallBlock : public Block
{
public:
	WallBlock(glm::vec2 position, int blockSize);
	virtual bool blocks() { return true; };

	virtual void draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet );

};