#pragma once
#include "../../PlayAllTheGames/AlivePlayer.h"

namespace backattack
{
	class Player : public AlivePlayer
	{
	public:
		Player(int index) : AlivePlayer(index)
		{

		}
	};
}
