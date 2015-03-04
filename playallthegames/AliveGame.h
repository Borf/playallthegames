#pragma once

#include "Game.h"
#include <blib/linq.h>

template<class PlayerClass>
class AliveGame : public Game<PlayerClass>
{
public:
	virtual bool hasWinner()
	{
		return blib::linq::count(players, [] (PlayerClass* p) { return p->alive; }) <= 1;
	}
	virtual std::list<Player*> getWinners()
	{
		return blib::linq::where<std::list<Player*> >(players, [] (PlayerClass* p) { return p->alive; });
	}
};
