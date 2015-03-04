#include "OrderedGameSelector.h"



OrderedGameSelector::OrderedGameSelector( std::vector<GameBase*> games )
{
	next = 0;
	this->games = games;
}

GameBase* OrderedGameSelector::nextGame()
{
	int n = next;
	next = (next + 1) % games.size();
	return games[n];
}
