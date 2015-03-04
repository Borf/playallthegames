#pragma once

#include "../MenuScreen.h"

class PlayAllTheGames;
class User;
class KeyboardMenuItem;
class ColorCycleMenuItem;

class UsersEditMenu : public MenuScreen
{
	KeyboardMenuItem* name;
	User* user;
public:
	UsersEditMenu(User* user);
	ColorCycleMenuItem* color;


	void back(PlayAllTheGames* playallthegames);

};