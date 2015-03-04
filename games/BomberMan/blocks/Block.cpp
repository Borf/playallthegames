#include "Block.h"

#include <blib/SpriteBatch.h>
#include <blib/SpriteSheet.h>
#include <blib/Math.h>


Block::Block(glm::vec2 position, int blockSize) : rect(position, blockSize, blockSize)
{
	this->position = position;
	this->blockSize = blockSize;
}

Block::~Block()
{

}

void Block::draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet, int index )
{
	float scale = blockSize / (float)spriteSheet->spriteHeight;
	spriteBatch->draw(spriteSheet->getFrame(index,0), blib::math::easyMatrix(position, 0, glm::vec2(scale,scale)));
}

