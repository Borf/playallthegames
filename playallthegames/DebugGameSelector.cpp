#include "DebugGameSelector.h"

#include "Game.h"


DebugGameSelector::DebugGameSelector( GameBase* game )
{
	this->game = game;
}


GameBase* DebugGameSelector::nextGame()
{
	return game;
}

