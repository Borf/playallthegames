#pragma once

#include "../../playallthegames/Player.h"

class TallyTulipPlayer : public Player
{
public:
	int count;
	float jump;
	TallyTulipPlayer(int index) : Player(index)
	{
		count = 0;
		jump = 0;
	}

};

