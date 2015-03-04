#pragma once

#include "AutomaticGameSelector.h"

class DebugGameSelector : public AutomaticGameSelector
{
	GameBase* game;
public:
	DebugGameSelector(GameBase* game);
	virtual GameBase* nextGame();
};
