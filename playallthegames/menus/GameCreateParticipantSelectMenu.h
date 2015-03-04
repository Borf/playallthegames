#pragma once

#include "../MenuScreen.h"
#include <vector>

class PlayAllTheGames;
class OptionMenuItem;
class Session;
class ToggleMenuItem;

class GameCreateParticipantSelectMenu : public MenuScreen
{
	std::vector<ToggleMenuItem*> players;
public:
	GameCreateParticipantSelectMenu(PlayAllTheGames* playallthegames);
	void back(PlayAllTheGames* playallthegames);
};
