#pragma once

#include "../MenuScreen.h"
#include <vector>

class PlayAllTheGames;
class OptionMenuItem;
class Session;
class ToggleMenuItem;

class GameCreateGameSelectMenu : public MenuScreen
{
	std::vector<ToggleMenuItem*> games;
public:
	GameCreateGameSelectMenu(PlayAllTheGames* playallthegames);
	void back(PlayAllTheGames* playallthegames);
};
