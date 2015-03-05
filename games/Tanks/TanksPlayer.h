#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

class b2Body;
class Settings;
class b2RevoluteJoint;

class TanksPlayer : public AlivePlayer
{
public:
	b2Body* body;
	b2Body* turret;
	b2RevoluteJoint* turretJoint;

	float targetAngle;
	float lastShootTime;

	TanksPlayer(int index);

	glm::mat4 getMatrix(Settings* settings);
	glm::mat4 getTurretMatrix(Settings* settings);

};