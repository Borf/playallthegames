#pragma once

#include "../MenuScreen.h"

class PlayAllTheGames;

class MainMenu : public MenuScreen
{
public:
	MainMenu();

	void quit(PlayAllTheGames* playallthegames);
	void openSettings(PlayAllTheGames* playallthegames);
	void start(PlayAllTheGames* playallthegames);
	void quickstart(PlayAllTheGames* playallthegames);
	void users(PlayAllTheGames* playallthegames);
	void stats(PlayAllTheGames* playallthegames);
};