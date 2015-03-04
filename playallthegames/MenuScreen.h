#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace blib { class ResourceManager; class Font; class Texture; class SpriteBatch; }

class MenuItem;
class Settings;

class MenuScreen
{
private:
	double lastAction;
	int lastActionId;
	int getAction(glm::vec2 joystick);

	double lastActionTime;


protected:
	std::string title;
public:
	static blib::Texture* texture;
	static blib::Font* font;
	static void loadResources(blib::ResourceManager* manager);

	std::string message;
	std::vector<MenuItem*> items;
	int selectedItem;

	MenuScreen();
	void draw(blib::SpriteBatch* spriteBatch, Settings* settings);
	
	void handleInput(glm::vec2 joystick);
	void handleKeyboard(char key);
	bool canType();
};

