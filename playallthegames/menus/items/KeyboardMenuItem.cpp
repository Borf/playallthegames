#include "KeyboardMenuItem.h"
#include "../../MenuScreen.h"

#include <blib/SpriteBatch.h>
#include <blib/Font.h>
#include <blib/Util.h>
#include <blib/util/Profiler.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

KeyboardMenuItem::KeyboardMenuItem(std::string title, std::string value) : MenuItem(title)
{
	this->value = value;
}


void KeyboardMenuItem::draw(blib::SpriteBatch* spriteBatch, glm::vec2 pos, glm::vec2 size, float selected)
{
	if(selected >= 0)
		spriteBatch->drawStretchyRect(MenuScreen::texture, glm::translate(glm::mat4(), glm::vec3(pos,0)), blib::math::Rectangle(0,0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), size, glm::clamp((1+0.2f * (float)glm::cos(selected*10)) * glm::vec4(0.5,0.5,0.8,1), 0, 1));
	else
		spriteBatch->drawStretchyRect(MenuScreen::texture, glm::translate(glm::mat4(), glm::vec3(pos,0)), blib::math::Rectangle(0,0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), size, glm::vec4(1,1,1,1));

	{
		float textLen = MenuScreen::font->textlen(title);
		float scale = (size.x-155) / textLen;
		if(scale > 2)
			scale = 2;

		spriteBatch->draw(MenuScreen::font, title, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos.x+size.x/2 - scale*textLen/2, pos.y-scale*40 + size.y/4,0)), glm::vec3(scale, scale, 1)), glm::vec4(1,1,1,1));
	}
	{
		float textLen = MenuScreen::font->textlen(value);
		float scale = (size.x-155) / textLen;
		if(scale > 2.5f)
			scale = 2.5f;

		spriteBatch->draw(MenuScreen::font, value, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos.x+size.x/2 - scale*textLen/2, pos.y-scale*40 + 2.5f*size.y/4,0)), glm::vec3(scale, scale, 1)), glm::vec4(1,1,1,1));
	}
}


void KeyboardMenuItem::onChar(char key)
{
	if(key >= ' ')
		value += key;
	if(key == 8 && value.size() > 0)
		value = value.substr(0, value.size()-1);

}