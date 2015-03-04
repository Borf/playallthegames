#pragma once

#include "Block.h"


class DestructableBlock : public Block
{
public:
	DestructableBlock(glm::vec2 position, int blockSize);


	virtual bool canExplode() { return true; }
	virtual bool blocks() { return true; };
	virtual void draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet );

};