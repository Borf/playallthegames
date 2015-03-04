#pragma once

#include "../MenuScreen.h"

class PlayAllTheGames;
class OptionMenuItem;
class Session;

class GameCreateMenu : public MenuScreen
{
	OptionMenuItem* playerCount;
	OptionMenuItem* playerSelector;
	OptionMenuItem* gameSelector;
	OptionMenuItem* victoryCondition;
	OptionMenuItem* difficulty;
public:
	GameCreateMenu(Session* session);
	void back(PlayAllTheGames* playallthegames);
	void start(PlayAllTheGames* playallthegames);
	void players(PlayAllTheGames* playallthegames);
	void games(PlayAllTheGames* playallthegames);

	void setSettings(Session* session);
};
