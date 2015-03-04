#include "SpeedPowerUp.h"
#include "../../BombermanPlayer.h"

SpeedPowerUp::SpeedPowerUp( glm::vec2 position, int blockSize ) : PowerUp(position, blockSize)
{

}

bool SpeedPowerUp::onTouch( BomberManPlayer* player )
{
	player->speed++;
	return true;
}

void SpeedPowerUp::draw( blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet )
{
	PowerUp::draw(spriteBatch, spriteSheet, 5);
}
