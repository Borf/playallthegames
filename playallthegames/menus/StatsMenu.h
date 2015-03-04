#pragma once

#include "../MenuScreen.h"

class PlayAllTheGames;
class Settings;
class OptionMenuItem;

class StatsMenu : public MenuScreen
{
public:
	StatsMenu();

	void back(PlayAllTheGames* playallthegames);
	std::vector<std::string> getMostGamesPlayed();
	std::vector<std::string> getMostGamesWon();
	std::vector<std::string> getWinRatio();
};