#include "WallBlock.h"



WallBlock::WallBlock( glm::vec2 position, int blockSize ) : Block(position, blockSize)
{

}

void WallBlock::draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet )
{
	Block::draw(spriteBatch, spriteSheet, 2);
}

