#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

#include <glm/glm.hpp>

class ZombieSurvivalPlayer : public AlivePlayer
{
public:
	glm::vec2 position;
	float rotation;

	ZombieSurvivalPlayer(int index);

};