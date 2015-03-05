#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"

#include <vector>


class b2Body;
class Wheel;
class b2World;

class MacroMachinesPlayer : public ScorePlayer
{
public:
	enum class Drive
	{
		FWD,
		RWD,
		AWD,
	};
	b2Body* body;

	float Width = 1;
	float Height = 2;

	std::vector<Wheel*> wheels;
	bool forward;
	bool braking;

	float steering;


	float max_speed = 40;
	float power = 150;

	int levelIndex;


	MacroMachinesPlayer(int index);

	void spawn(b2World* world, glm::vec2 position, Drive drive, float rotation);
	float getSpeed();
	glm::vec2 getLocalVelocity();
	void update(float elapsedTime);


};

