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

		float rotations;
		float lastAngle;
		float maxSpeed;
		float rotationSpeed;
		int loops;


		HillClimbPlayer(int index) : AlivePlayer(index)
		{
			lastX = -100;
			maxSpeed = 50;
			loops = 0;
			rotations = 0;
			lastAngle = 0;
			rotationSpeed = 200;
		}
	};


}