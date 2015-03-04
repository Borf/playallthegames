#pragma once

#include "Player.h"

class ScorePlayer : public Player
{
public:
	int score;
	ScorePlayer(int index) : Player(index)
	{
		score = 0;
	}
};