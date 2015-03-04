#pragma once

#include "../MenuScreen.h"

class PlayAllTheGames;


class UsersMenu : public MenuScreen
{
public:
	UsersMenu();


	void back(PlayAllTheGames* playallthegames);
	void newuser(PlayAllTheGames* playallthegames);
	void edituser(PlayAllTheGames* playallthegames);

};