#pragma  once

#include "../PowerUp.h"

class PushPowerUp : public PowerUp
{
public:
	PushPowerUp(glm::vec2 position, int blockSize);

	bool onTouch(BomberManPlayer* player);

	void draw(blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet);

};

