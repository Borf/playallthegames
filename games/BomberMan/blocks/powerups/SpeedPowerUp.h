#pragma  once

#include "../PowerUp.h"

class SpeedPowerUp : public PowerUp
{
public:
	SpeedPowerUp(glm::vec2 position, int blockSize);

	bool onTouch(BomberManPlayer* player);

	void draw(blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet);

};

