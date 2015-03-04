#pragma once

#include "MenuItem.h"
#include <vector>

class ColorCycleMenuItem : public MenuItem
{
public:
	float hue;


	ColorCycleMenuItem(std::string title, float hue = 0);

	void nextItem(PlayAllTheGames* playallthegames);

	virtual void draw(blib::SpriteBatch* spriteBatch, glm::vec2 pos, glm::vec2 size, float selected);

};