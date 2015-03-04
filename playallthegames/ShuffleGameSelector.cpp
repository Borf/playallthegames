#include "ShuffleGameSelector.h"

#include <blib/Math.h>

ShuffleGameSelector::ShuffleGameSelector(std::vector<GameBase*> games)
{
	next = 0;
	this->games = games;
}

GameBase* ShuffleGameSelector::nextGame()
{
	if (next == 0)
	{
		games = blib::math::permute(games);
	}

	int n = next;
	next = (next + 1) % games.size();
	return games[n];
}
