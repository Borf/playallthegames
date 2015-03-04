#pragma once

#include "../../ScorePlayer.h"

class b2Body;

class PoolPlayer : public ScorePlayer
{
public:
	PoolPlayer(int index);

	b2Body* ball;

	float direction;

};

