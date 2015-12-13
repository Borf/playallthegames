#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"
class b2Body;
class b2RevoluteJoint;

namespace hillclimb
{

	class HillClimbPlayer : public AlivePlayer
	{
	public:

		b2Body* car;
		b2Body* frontWheel;
		b2Body* backWheel;

		b2RevoluteJoint* frontEngine;
		b2RevoluteJoint* backEngine;

		float lastX;
		float lastTime;


		HillClimbPlayer(int index) : AlivePlayer(index)
		{
			lastX = -100;


		}
	};


}