#pragma once

#include <glm/glm.hpp>
#include <blib/math/Rectangle.h>

namespace blib { class SpriteBatch; class SpriteSheet; }
class BomberManPlayer;

class Block
{
public:
	Block(glm::vec2 position, int blockSize);
	virtual ~Block();

	glm::vec2 position;
	int blockSize;

	blib::math::Rectangle rect;


	virtual bool canExplode() { return false; }
	virtual bool blocks() { return false; };
	virtual bool onTouch(BomberManPlayer* player) {return false; }


	virtual void draw(blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet) = 0;
	virtual void draw(blib::SpriteBatch* spriteBatch, blib::SpriteSheet* spriteSheet, int index);
};