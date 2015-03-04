#include "ColorCycleMenuItem.h"
#include "../../MenuScreen.h"

#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/Font.h>
#include <blib/util/Profiler.h>
#include <blib/Util.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


ColorCycleMenuItem::ColorCycleMenuItem(std::string title, float hue) : MenuItem(title, std::bind(&ColorCycleMenuItem::nextItem, this, std::placeholders::_1))
{
	this->hue = hue;
}

void ColorCycleMenuItem::nextItem(PlayAllTheGames* playallthegames)
{

}

void ColorCycleMenuItem::draw(blib::SpriteBatch* spriteBatch, glm::vec2 pos, glm::vec2 size, float selected)
{
	glm::vec3 color = blib::util::hsv2rgb(glm::vec3(hue, 1, 1));
	if (selected >= 0)
		spriteBatch->drawStretchyRect(MenuScreen::texture, glm::translate(glm::mat4(), glm::vec3(pos, 0)), blib::math::Rectangle(0, 0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), size, glm::clamp((1 + 0.2f * (float)glm::cos(selected * 10)) * glm::vec4(color, 1), 0, 1));
	else
		spriteBatch->drawStretchyRect(MenuScreen::texture, glm::translate(glm::mat4(), glm::vec3(pos, 0)), blib::math::Rectangle(0, 0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), size, glm::vec4(color, 1));

	float textLen = MenuScreen::font->textlen(title);
	float scale = (size.x - 155) / textLen;
	if (scale > 2)
		scale = 2;

	spriteBatch->draw(MenuScreen::font, title, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos.x + size.x / 2 - scale*textLen / 2, pos.y - scale * 40 + size.y / 3, 0)), glm::vec3(scale, scale, 1)), glm::vec4(1,1,1,1));

	std::string ColorCycle = blib::util::toString(hue);
	spriteBatch->draw(MenuScreen::font, ColorCycle, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos.x + size.x / 2 - MenuScreen::font->textlen(ColorCycle) / 2, pos.y + size.y / 4 + 50, 0)), glm::vec3(1, 1, 1)), glm::vec4(1, 1, 1, 1));


}

