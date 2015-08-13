#include "Session.h"

#include "UserDb.h"
#include "PlayerSelector.h"
#include "Participant.h"
#include "User.h"
#include "Game.h"
#include "Player.h"
#include "PlayAllTheGames.h"

#include "OrderedGameSelector.h"
#include "DebugGameSelector.h"
#include "ShuffleGameSelector.h"

#include "SessionStats.h"
#include "StatsDb.h"

#include <blib/Color.h>
#include <blib/Util.h>
#include <blib/util/Log.h>
#include <blib/linq.h>

#include <set>
#include <glm/glm.hpp>


using blib::util::Log;

Session::Session()
{
	playerSelector = NULL;
	gameSelectorType = NULL;
	playerCount = 4;
	playerSelectorType = 0;
	gameSelectorType = 0;
	difficulty = Difficulty::Normal;
	stats = NULL;

	stats = new SessionStats();
	StatsDb::getInstance()->insert(stats);

}

void Session::setDebug(PlayAllTheGames* playallthegames, GameBase* game)
{
	participants.clear();
	
	playerCount = 4;
	participants = blib::linq::select<std::vector<Participant*> >(UserDb::getInstance()->getRandomPlayers(playerCount), [](User* u) { return new Participant(u); });
	setPlayerColors();
	playerSelector = new FixedPlayerSelector(participants);
	gameSelector = new DebugGameSelector(game);

	playerSelector->setInitialPlayers(participants);
}



void Session::nextGame( PlayAllTheGames* playallthegames )
{
	if (playallthegames->activeGame)
	{
		SessionStats::Result* result = new SessionStats::Result();
		result->finished = playallthegames->activeGame->hasWinner();
		result->participants = blib::linq::select<std::vector<User*> >(playallthegames->activeGame->getPlayers(), [](Player* player) { return player->participant->user;  });
		result->winners = blib::linq::select<std::vector<User*> >(playallthegames->activeGame->getWinners(), [](Player* player) { return player->participant->user;  });
		result->gameName = playallthegames->activeGame->getName();
		result->timeTaken = (int)(playallthegames->activeGame->gameTime * 1000);
		stats->results.push_back(result);

		StatsDb::getInstance()->saveToFile();
	}
	else
	{
		stats->users = blib::linq::select<std::vector<User*> >(participants, [](Participant* participant) { return participant->user; });
	}

	playallthegames->activeGame = gameSelector->nextGame();
	if(!playallthegames->activeGame->resourcesLoaded)
	{
		playallthegames->activeGame->loadResources();
		playallthegames->activeGame->resourcesLoaded = true;
	}

	playallthegames->activeGame->setPlayers(playerSelector->nextPlayers(playerCount));
	playallthegames->activeGame->gameTime = 0;
	playallthegames->activeGame->start(difficulty);
}


void Session::setPlayerColors()
{
	std::set<glm::vec4, blib::util::compareVec4> colors;
	colors.insert(blib::Color::red);
	colors.insert(blib::Color::green);
	colors.insert(blib::Color::blue);
	colors.insert(blib::Color::purple);
	std::set<glm::vec4, blib::util::compareVec4> colorsTaken;
	std::set<Participant*> noColor;
	for(size_t i = 0; i < participants.size(); i++)
	{
		glm::vec4 color = participants[i]->user->preferredColor;
		if(colorsTaken.find(color) == colorsTaken.end())
		{
			colorsTaken.insert(color);
			participants[i]->color = color;
		}
		else
			noColor.insert(participants[i]);
		if(colors.find(color) != colors.end())
			colors.erase(colors.find(color));
	}

	for(std::set<Participant*>::iterator it = noColor.begin(); it != noColor.end(); it++)
	{
		(*it)->color = *colors.begin();
		colors.erase(colors.begin());
	}
}

void Session::start()
{
	setPlayerColors();

	games = blib::linq::where(games, [this](GameBase* game) { return playerCount >= game->getPlayerCount().first && playerCount <= game->getPlayerCount().second;  });


	if (gameSelectorType == 0)
		gameSelector = new OrderedGameSelector(games);
	else if (gameSelectorType == 1)
		gameSelector = new ShuffleGameSelector(games);
	else
	{
		Log::out<<"Unknown gameselector: "<<gameSelectorType<<Log::newline;
		gameSelector = new OrderedGameSelector(games);
	}

	if(playerSelectorType == 0)
		playerSelector = new FairPlayerSelector(participants);
	else
	{
		Log::out<<"Unknown playerselector: "<<playerSelectorType<<Log::newline;
		playerSelector = new FixedPlayerSelector(participants);
	}


	participantCursors.resize(participants.size(), glm::vec2(1920, 1080) / 2.0f);
	participantIndex.resize(participants.size(), -1);
}

void Session::setInitialPlayers()
{
	std::vector<Participant*> players;
	for (size_t i = 0; i < participantIndex.size(); i++)
		players.push_back(participants[participantIndex[i]]);
	playerSelector->setInitialPlayers(players);
}
