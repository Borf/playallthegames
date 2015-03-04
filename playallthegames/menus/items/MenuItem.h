#pragma once

#include <string>
#include <functional>
#include <glm/glm.hpp>

namespace blib { class SpriteBatch; };
class PlayAllTheGames;

class MenuItem
{
public:
	std::string title;
	std::function<void(PlayAllTheGames*)> action;
	

	glm::vec4 color;
	
	MenuItem(std::string title, std::function<void(PlayAllTheGames*)> action = std::function<void(PlayAllTheGames*)>());
	virtual void draw(blib::SpriteBatch* spriteBatch, glm::vec2 pos, glm::vec2 size, float selected);
	virtual inline bool canType() { return false; };
	virtual void onChar(char value) { };
};
