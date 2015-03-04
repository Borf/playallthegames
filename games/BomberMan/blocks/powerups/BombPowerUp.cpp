#include "BombPowerUp.h"

#include "../../BombermanPlayer.h"

void BombPowerUp::draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet )
{
	PowerUp::draw(spriteBatch, spriteSheet, 3);
}

bool BombPowerUp::onTouch( BomberManPlayer* player )
{
	player->bombs++;
	return true;
}

BombPowerUp::BombPowerUp( glm::vec2 position, int blockSize ) : PowerUp(position, blockSize)
{

}

