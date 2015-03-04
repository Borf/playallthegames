#include "PushPowerUp.h"
#include "../../BombermanPlayer.h"

PushPowerUp::PushPowerUp( glm::vec2 position, int blockSize ) : PowerUp(position, blockSize)
{

}

bool PushPowerUp::onTouch( BomberManPlayer* player )
{
	player->pushBombs = true;
	return true;
}

void PushPowerUp::draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet )
{
	PowerUp::draw(spriteBatch, spriteSheet, 6);
}
