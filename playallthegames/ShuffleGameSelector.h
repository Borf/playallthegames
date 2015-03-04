#pragma once

#include "GameSelector.h"

#include <vector>

class ShuffleGameSelector : public GameSelector
{
	std::vector<GameBase*> games;
	int next;
public:
	ShuffleGameSelector(std::vector<GameBase*> games);
	virtual GameBase* nextGame();
};