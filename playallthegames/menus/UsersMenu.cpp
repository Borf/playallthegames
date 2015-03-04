#include "UsersMenu.h" 
#include "items/MenuItem.h"

#include "MainMenu.h"
#include "UsersEditMenu.h"

#include "../PlayAllTheGames.h"
#include "../User.h"
#include "../UserDb.h"


UsersMenu::UsersMenu()
{
	title = "Users";

	std::vector<User*> users = UserDb::getInstance()->data;
	for(size_t i = 0; i < users.size(); i++)
	{
		MenuItem* item = new MenuItem(users[i]->name, std::bind(&UsersMenu::edituser, this, std::placeholders::_1));
		item->color = users[i]->preferredColor;
		items.push_back(item);
	}
	
	items.push_back(new MenuItem("New", std::bind(&UsersMenu::newuser, this, std::placeholders::_1)));
	items.push_back(new MenuItem("Back", std::bind(&UsersMenu::back, this, std::placeholders::_1)));
}



void UsersMenu::back(PlayAllTheGames* playallthegames)
{
	playallthegames->activeMenu = new MainMenu();
}

void UsersMenu::newuser(PlayAllTheGames* playallthegames)
{
	playallthegames->activeMenu = new UsersEditMenu(NULL);
}

void UsersMenu::edituser(PlayAllTheGames* playallthegames)
{
	std::vector<User*> users = UserDb::getInstance()->data;
	playallthegames->activeMenu = new UsersEditMenu(users[selectedItem]);
}