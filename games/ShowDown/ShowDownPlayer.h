#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

#include <glm/glm.hpp>

class ShowDownPlayer : public AlivePlayer
{
public:
	glm::vec2 position;
	float rotation;

	ShowDownPlayer(int index);

};