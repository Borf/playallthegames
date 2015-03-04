#pragma once

#include "../../ScorePlayer.h"

class BreakOutBall;

class BreakOutPlayer : public ScorePlayer
{
public:
	BreakOutPlayer(int index);

	float position;
	BreakOutBall* ball;
};