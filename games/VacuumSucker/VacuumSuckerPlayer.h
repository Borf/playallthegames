#pragma once

#include "../../ScorePlayer.h"

#include <glm/glm.hpp>

class VacuumSuckerPlayer : public ScorePlayer
{
public:
	glm::vec2 position;
	float rotation;

	VacuumSuckerPlayer(int index);

};