#pragma  once

#include "../PowerUp.h"

class BombPowerUp : public PowerUp
{
public:
	BombPowerUp(glm::vec2 position, int blockSize);

	bool onTouch(BomberManPlayer* player);

	void draw(blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet);

};

