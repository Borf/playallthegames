#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"

class PokerStepsPlayer : public ScorePlayer
{
public:
	PokerStepsPlayer(int index);

	float posLeft;
	int choice;

};