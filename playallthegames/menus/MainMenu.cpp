#include "MainMenu.h"
#include "SettingsMenu.h"
#include "GameCreateMenu.h"
#include "UsersMenu.h"
#include "StatsMenu.h"
#include "items/MenuItem.h"
#include "items/OptionMenuItem.h"

#include "../PlayAllTheGames.h"
#include "../Session.h"
#include "../game.h"
#include "../Participant.h"
#include "../ShuffleGameSelector.h"
#include "../PlayerSelector.h"
#include "../UserDb.h"
#include <blib/linq.h>



MainMenu::MainMenu()
{
	title = "Play All The Games";

	std::vector<std::string> playerCount;
	playerCount.push_back("1 Player");
	playerCount.push_back("2 Player");
	playerCount.push_back("3 Player");
	playerCount.push_back("4 Player");

	items.push_back(new MenuItem("Start",		std::bind(&MainMenu::start, this, std::placeholders::_1)));
	items.push_back(new MenuItem("Quick Start", std::bind(&MainMenu::quickstart, this, std::placeholders::_1)));
	items.push_back(new MenuItem("Users",		std::bind(&MainMenu::users, this, std::placeholders::_1)));
	items.push_back(new MenuItem("Statistics",	std::bind(&MainMenu::stats, this, std::placeholders::_1)));
	items.push_back(new MenuItem("Settings",	std::bind(&MainMenu::openSettings, this, std::placeholders::_1)));
	items.push_back(new MenuItem("Quit",		std::bind(&MainMenu::quit, this, std::placeholders::_1)));
}

void MainMenu::quit( PlayAllTheGames* playallthegames )
{
	playallthegames->quit();
}

void MainMenu::openSettings( PlayAllTheGames* playallthegames )
{
	playallthegames->activeMenu = new SettingsMenu(playallthegames->settings);
}

void MainMenu::start(PlayAllTheGames* playallthegames)
{
	if (playallthegames->session)
		delete playallthegames->session;
	playallthegames->session = new Session();
	playallthegames->session->games = playallthegames->games;
	playallthegames->activeMenu = new GameCreateMenu(playallthegames->session);
}

void MainMenu::quickstart(PlayAllTheGames* playallthegames)
{
	if (playallthegames->session)
		delete playallthegames->session;
	playallthegames->session = new Session();
	playallthegames->session->games = playallthegames->games;
	playallthegames->session->playerCount = 4;
	playallthegames->session->difficulty = Difficulty::Normal;
	playallthegames->session->gameSelectorType = 1;
	playallthegames->session->participants = blib::linq::select<std::vector<Participant*> >(UserDb::getInstance()->getRandomPlayers(4), [](User* u) { return new Participant(u); });
	playallthegames->session->setPlayerColors();

	playallthegames->session->start();
	playallthegames->session->playerSelector->setInitialPlayers(playallthegames->session->participants);

	playallthegames->switchState(PlayAllTheGames::PreGame);
	playallthegames->session->nextGame(playallthegames);

}

void MainMenu::users(PlayAllTheGames* playallthegames)
{
	playallthegames->activeMenu = new UsersMenu();
}

void MainMenu::stats(PlayAllTheGames* playallthegames)
{
	playallthegames->activeMenu = new StatsMenu();
}