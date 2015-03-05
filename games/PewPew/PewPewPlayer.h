#pragma once

#include "../../PlayAllTheGames/Player.h"

class b2Body;

class PewPewPlayer : public Player
{
public:
	PewPewPlayer(int index);

	b2Body* body;
};

