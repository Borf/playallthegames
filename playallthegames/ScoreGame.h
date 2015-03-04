#pragma once

#include "Game.h"
#include <blib/linq.h>

template<class PlayerType>
class ScoreGame : public Game<PlayerType>
{
protected:
	int totalScore;
	int maxPlayerScore;
public:
	ScoreGame()
	{
		totalScore = -1;
		maxPlayerScore = -1;
	}

	virtual bool hasWinner()
	{
		if(totalScore > 0)
			return blib::linq::sum<int>(players, [] (PlayerType* p) { return p->score; }) == totalScore;
		if(maxPlayerScore > 0)
		{
			return blib::linq::any(players, [this] (PlayerType* p) { return p->score >= maxPlayerScore; });
		}
		return false;
	}

	virtual std::list<Player*> getWinners()
	{
		int maxScore = blib::linq::max<int>(players, [] (PlayerType* p) { return p->score; });
		return blib::linq::where<std::list<Player*> >(players, [maxScore] (PlayerType* p) { return p->score == maxScore; });
	}
};