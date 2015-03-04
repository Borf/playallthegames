#include "OptionMenuItem.h"
#include "../../MenuScreen.h"

#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/Font.h>
#include <blib/util/Profiler.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


OptionMenuItem::OptionMenuItem( std::string title, std::vector<std::string> options, int selectedOption /*= 0*/ ) : MenuItem(title,std::bind(&OptionMenuItem::nextItem, this, std::placeholders::_1))
{
	this->options = options;
	this->selectedOption = selectedOption;
}

void OptionMenuItem::nextItem( PlayAllTheGames* playallthegames )
{
	selectedOption = (selectedOption+1) % options.size();
}

void OptionMenuItem::draw( blib::SpriteBatch* spriteBatch, glm::vec2 pos, glm::vec2 size, float selected )
{
	if(selected >= 0)
		spriteBatch->drawStretchyRect(MenuScreen::texture, glm::translate(glm::mat4(), glm::vec3(pos,0)), blib::math::Rectangle(0,0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), size, glm::clamp((1+0.2f * (float)glm::cos(selected*10)) * glm::vec4(0.5,0.5,0.8,1), 0, 1));
	else
		spriteBatch->drawStretchyRect(MenuScreen::texture, glm::translate(glm::mat4(), glm::vec3(pos,0)), blib::math::Rectangle(0,0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), size, glm::vec4(1,1,1,1));


	float textLen = MenuScreen::font->textlen(title);
	float scale = (size.x-155) / textLen;
	if(scale > 2)
		scale = 2;

	spriteBatch->draw(MenuScreen::font, title, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos.x+size.x/2 - scale*textLen/2, pos.y-scale*40 + size.y/3,0)), glm::vec3(scale, scale, 1)), glm::vec4(1,1,1,1));	

	std::string option = "« " + options[selectedOption] + " »";
	spriteBatch->draw(MenuScreen::font, option, glm::scale(glm::translate(glm::mat4(), glm::vec3(pos.x+size.x/2 - MenuScreen::font->textlen(option)/2, pos.y + size.y/4 +50,0)), glm::vec3(1, 1, 1)), glm::vec4(1,1,1,1));	
}

