#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

class b2Body;

class SoccerPlayer : public AlivePlayer
{
public:
	b2Body* players[2];
	b2Body* bottle;
	SoccerPlayer(int index) : AlivePlayer(index)
	{

	}
};