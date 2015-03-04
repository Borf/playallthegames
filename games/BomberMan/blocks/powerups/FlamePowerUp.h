#pragma  once

#include "../PowerUp.h"

class FlamePowerUp : public PowerUp
{
public:
	FlamePowerUp(glm::vec2 position, int blockSize);

	bool onTouch(BomberManPlayer* player);

	void draw(blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet);

};

