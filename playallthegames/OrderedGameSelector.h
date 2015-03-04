#pragma once

#include "AutomaticGameSelector.h"

#include <vector>

class OrderedGameSelector : public AutomaticGameSelector
{
	std::vector<GameBase*> games;
	int next;
public:
	OrderedGameSelector(std::vector<GameBase*> games);
	virtual GameBase* nextGame();
};