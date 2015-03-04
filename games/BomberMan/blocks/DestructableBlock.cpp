#include "DestructableBlock.h"

DestructableBlock::DestructableBlock( glm::vec2 position, int blockSize ) : Block(position, blockSize)
{

}

void DestructableBlock::draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet )
{
	Block::draw(spriteBatch, spriteSheet, 0);
}

