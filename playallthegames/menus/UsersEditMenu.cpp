#include "UsersEditMenu.h" 
#include "items/MenuItem.h"
#include "items/KeyboardMenuItem.h"
#include "items/ColorCycleMenuItem.h"

#include "UsersMenu.h"

#include "../PlayAllTheGames.h"
#include "../User.h"
#include "../UserDb.h"

#include <blib/Util.h>

UsersEditMenu::UsersEditMenu(User* user)
{
	this->user = user;

	title = "Edit user";
	if(user != NULL)
		title += " " + user->name;
	
	items.push_back(name = new KeyboardMenuItem("Name", user ? user->name : ""));
	items.push_back(color = new ColorCycleMenuItem("Color", user ? blib::util::rgb2hsv(glm::vec3(user->preferredColor)).x : rand()%360));


	items.push_back(new MenuItem("Delete", std::bind(&UsersEditMenu::back, this, std::placeholders::_1)));
	items.push_back(new MenuItem("Back", std::bind(&UsersEditMenu::back, this, std::placeholders::_1)));
}



void UsersEditMenu::back(PlayAllTheGames* playallthegames)
{
	if(!user)
	{
		user = new User(name->value, glm::vec4(0,0,0,1));
		UserDb::getInstance()->insert(user);
	}

	user->name = name->value;
	user->preferredColor = glm::vec4(blib::util::hsv2rgb(glm::vec3(color->hue, 1, 1)), 1.0f);


	UserDb::getInstance()->saveToFile();

	playallthegames->activeMenu = new UsersMenu();
}
