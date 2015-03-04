#pragma once

#include "MenuItem.h"
#include <vector>

class OptionMenuItem : public MenuItem
{
public:
	std::vector<std::string> options;
	int selectedOption;

	OptionMenuItem(std::string title, std::vector<std::string> options, int selectedOption = 0);

	void nextItem(PlayAllTheGames* playallthegames);

	virtual void draw( blib::SpriteBatch* spriteBatch, glm::vec2 pos, glm::vec2 size, float selected );

};