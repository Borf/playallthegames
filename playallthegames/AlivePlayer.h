#pragma once

#include "Player.h"

class AlivePlayer : public Player
{
public:
	AlivePlayer(int index) : Player(index){ alive = true; }
	bool alive;
};