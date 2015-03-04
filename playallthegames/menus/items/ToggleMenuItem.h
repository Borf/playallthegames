#pragma once

#include "MenuItem.h"
#include <vector>

class ToggleMenuItem : public MenuItem
{
public:
	bool enabled;

	ToggleMenuItem(std::string title, bool enabled = false);

	void toggle(PlayAllTheGames* playallthegames);

	virtual void draw( blib::SpriteBatch* spriteBatch, glm::vec2 pos, glm::vec2 size, float selected );

};