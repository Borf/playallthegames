#include "EmptyBlock.h"


EmptyBlock::EmptyBlock( glm::vec2 position, int blockSize ) : Block(position, blockSize)
{
}

void EmptyBlock::draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet )
{
	Block::draw(spriteBatch, spriteSheet, 1);
}

