#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

#include <list>

class CurveFeverPlayer : public AlivePlayer
{
public:
	CurveFeverPlayer(int index);

	glm::vec2 prevPosition;
	glm::vec2 position;
	float direction;


	double ink;


	std::list<glm::vec2> trail;
};