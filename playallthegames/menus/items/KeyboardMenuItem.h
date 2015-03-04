#pragma once

#include "MenuItem.h"


class KeyboardMenuItem : public MenuItem
{
public:
	std::string value;

	KeyboardMenuItem(std::string title, std::string value);
	virtual inline bool canType() { return true; };
	virtual void onChar(char value);

	virtual void draw(blib::SpriteBatch* spriteBatch, glm::vec2 pos, glm::vec2 size, float selected);


};