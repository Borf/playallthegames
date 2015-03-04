#include "FlamePowerUp.h"

#include "../../BombermanPlayer.h"

FlamePowerUp::FlamePowerUp( glm::vec2 position, int blockSize ) : PowerUp(position, blockSize)
{

}

bool FlamePowerUp::onTouch( BomberManPlayer* player )
{
	player->flameLength++;
	return true;
}

void FlamePowerUp::draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet )
{
	PowerUp::draw(spriteBatch, spriteSheet, 4);
}
