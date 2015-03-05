#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

class b2Body;

class PushOutPlayer : public AlivePlayer
{
public:
	PushOutPlayer(int index);
	b2Body* body;
};