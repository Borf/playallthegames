#pragma once

#include <glm/glm.hpp>

class b2Body;
class b2World;
class MacroMachinesPlayer;

class Wheel
{
public:
	b2Body* body;
	float Width = 0.3f;
	float Height = 0.7f;
	glm::vec2 pos;

	float revolvePower;
	bool powered;
	MacroMachinesPlayer* player;


	glm::vec2 lastSlipSpot;



	Wheel(b2World* world, MacroMachinesPlayer* player, const glm::vec2 &position, bool powered, float revolvePower);

	glm::vec2 getLocalVelocity();
	glm::vec2 getDirectionVector();
	glm::vec2 getKillVelocityVector();
	void killSidewaysVelocity();
	void DampSidewaysVelocity(float elapsedTime);

	void setDensity(float density);

};