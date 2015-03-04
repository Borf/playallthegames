#include "MenuScreen.h"
#include "menus/items/MenuItem.h"
#include "Settings.h"

#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/ResourceManager.h>
#include <blib/Font.h>
#include <blib/math/Rectangle.h>
#include <blib/util/Profiler.h>
#include <blib/util/Log.h>

#include <glm/gtc/matrix_transform.hpp>

using blib::util::Log;

blib::Font* MenuScreen::font;
blib::Texture* MenuScreen::texture;


MenuScreen::MenuScreen()
{
	selectedItem = 0;
	lastAction = 0;
	lastActionId = -1;
	message = "";
	lastActionTime = 0;
}


void MenuScreen::loadResources( blib::ResourceManager* manager )
{
	texture = manager->getResource<blib::Texture>("assets/menu.png");
	font = manager->getResource<blib::Font>("menu");
}

void MenuScreen::draw( blib::SpriteBatch* spriteBatch, Settings* settings )
{
	spriteBatch->drawStretchyRect(texture, glm::translate(glm::mat4(), glm::vec3(10,10,0)), blib::math::Rectangle(0,0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), glm::vec2(settings->size.x-20,100), glm::vec4(1,1,1,1));
	spriteBatch->draw(font, title, glm::translate(glm::mat4(), glm::vec3(settings->center.x-font->textlen(title)/2,22,0)), glm::vec4(1,1,1,1));

	int count = items.size();

	int countX = (int)floor(sqrt(count));
	int countY = (int)ceil(count / (float)countX);

	int width = settings->resX;
	int height = settings->resY - 100;

	int itemWidth = width / countX;
	int itemHeight = height / countY;

	for(int i = 0; i < count; i++)
	{
		int x = 10 + (i%countX) * itemWidth;
		int y = 120+ (i/countX) * itemHeight;

		items[i]->draw(spriteBatch, glm::vec2(x, y), glm::vec2(itemWidth - 20, itemHeight - 20), i == selectedItem ? (float)(blib::util::Profiler::getAppTime() - lastActionTime) : -1.0f);
	}

	if(message != "")
	{
		float size = font->textlen(message)/2;
		spriteBatch->drawStretchyRect(texture, glm::translate(glm::mat4(), glm::vec3(settings->center.x - size-50,settings->center.y-100,0)), blib::math::Rectangle(0,0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), glm::vec2(size*2+100,200), glm::vec4(1,0.5f,0.5f,1));
		spriteBatch->draw(font, message, glm::translate(glm::mat4(), glm::vec3(settings->center.x-size,settings->center.y-22,0)), glm::vec4(1,1,1,1));
	}

}

int MenuScreen::getAction(glm::vec2 joystick)
{
	if(joystick.x > 0.5)
		return 1;
	else if(joystick.x < -0.5)
		return 2;
	if(joystick.y > 0.5)
		return 3;
	else if(joystick.y < -0.5)
		return 4;
	return 0;
}

void MenuScreen::handleInput( glm::vec2 joystick )
{
	if(lastAction+0.5 < blib::util::Profiler::getAppTime())
	{
		int action = getAction(joystick);
		if(action == 1)
			selectedItem = (selectedItem+1)%items.size();
		else if(action == 2)
			selectedItem = (selectedItem+items.size()-1)%items.size();
		if(action == 3)
		{
			int countX = (int)floor(sqrt(items.size()));
			selectedItem = (selectedItem+countX)%items.size();
		}
		else if(action == 4)
		{
			int countX = (int)floor(sqrt(items.size()));
			selectedItem = (selectedItem+items.size()-countX)%items.size();
		}


		if(action != 0)
		{
			lastActionTime = blib::util::Profiler::getAppTime();
			lastAction = blib::util::Profiler::getAppTime();
			lastActionId = action;
		}

	}

	if(lastActionId != getAction(joystick) && lastAction > 0)
		lastAction = 0;
}


void MenuScreen::handleKeyboard(char key)
{
	items[selectedItem]->onChar(key);
}


bool MenuScreen::canType()
{
	return items[selectedItem]->canType();
}