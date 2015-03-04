#include "GameCreateMenu.h"

#include "MainMenu.h"
#include "GameCreateGameSelectMenu.h"
#include "GameCreateParticipantSelectMenu.h"

#include "../PlayAllTheGames.h"
#include "../Session.h"
#include "items/MenuItem.h"
#include "items/OptionMenuItem.h"


GameCreateMenu::GameCreateMenu(Session* session)
{
	title = "Game Settings";

	std::vector<std::string> playerCounts;
	playerCounts.push_back("2 Players");
	playerCounts.push_back("3 Players");
	playerCounts.push_back("4 Players");
	playerCounts.push_back("5 Players");
	playerCounts.push_back("6 Players");
	playerCounts.push_back("7 Players");
	playerCounts.push_back("8 Players");

	std::vector<std::string> playerSelectors;
	playerSelectors.push_back("Fair");
	playerSelectors.push_back("Fixed");
	playerSelectors.push_back("Winner Loses");
	playerSelectors.push_back("Loser Loses");
	playerSelectors.push_back("Random");

	std::vector<std::string> gameSelectors;
	gameSelectors.push_back("In order");
	gameSelectors.push_back("Shuffle");
	gameSelectors.push_back("Random");
	gameSelectors.push_back("Loser Picks");
	gameSelectors.push_back("Winner Picks");
	gameSelectors.push_back("Everybody picks");

	std::vector<std::string> victoryConditions;
	victoryConditions.push_back("No Limit");
	victoryConditions.push_back("Time");
	victoryConditions.push_back("Score");
	victoryConditions.push_back("Rounds");

	std::vector<std::string> difficulties;
	difficulties.push_back("Easy");
	difficulties.push_back("Normal");
	difficulties.push_back("Hard");
	difficulties.push_back("Cruel");
	difficulties.push_back("WTF");


	items.push_back(playerCount = new OptionMenuItem("Player Count", playerCounts, session->playerCount-2));
	items.push_back(new MenuItem("Players", std::bind(&GameCreateMenu::players, this, std::placeholders::_1)));
	items.push_back(playerSelector = new OptionMenuItem("Player Selection", playerSelectors, session->playerSelectorType));
	items.push_back(new MenuItem("Games", std::bind(&GameCreateMenu::games, this, std::placeholders::_1)));
	items.push_back(gameSelector = new OptionMenuItem("Game Selection", gameSelectors, session->gameSelectorType));
	items.push_back(victoryCondition = new OptionMenuItem("Victory Condition", victoryConditions));
	items.push_back(difficulty = new OptionMenuItem("Difficulty", difficulties, 1));

	items.push_back(new MenuItem("Start", std::bind(&GameCreateMenu::start, this, std::placeholders::_1)));
	items.push_back(new MenuItem("Back", std::bind(&GameCreateMenu::back, this, std::placeholders::_1)));
}

void GameCreateMenu::back( PlayAllTheGames* playallthegames )
{
	playallthegames->activeMenu = new MainMenu();
	if(playallthegames->session)
	{
		delete playallthegames->session;
		playallthegames->session = NULL;
	}
}

void GameCreateMenu::start(PlayAllTheGames* playallthegames)
{
	setSettings(playallthegames->session);

	if(playallthegames->session->playerCount > (int)playallthegames->session->participants.size())
	{
		message = "Error: not enough players";
		return;
	}

	playallthegames->session->start();

	playallthegames->switchState(PlayAllTheGames::PlayerIdentify);
//	playallthegames->session->nextGame(playallthegames);

}

void GameCreateMenu::setSettings( Session* session )
{
	session->gameSelectorType = gameSelector->selectedOption;
	session->playerCount = playerCount->selectedOption+2;
	session->playerSelectorType = playerSelector->selectedOption;
	session->difficulty = (Difficulty)difficulty->selectedOption;
}


void GameCreateMenu::players(PlayAllTheGames* playallthegames)
{
	setSettings(playallthegames->session);
	playallthegames->activeMenu = new GameCreateParticipantSelectMenu(playallthegames);
}
void GameCreateMenu::games(PlayAllTheGames* playallthegames)
{
	setSettings(playallthegames->session);
	playallthegames->activeMenu = new GameCreateGameSelectMenu(playallthegames);
}
