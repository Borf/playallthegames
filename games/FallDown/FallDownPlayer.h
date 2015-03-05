#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

class FallDownPlayer : public AlivePlayer
{
public:
	glm::vec2 position;
	glm::vec2 speed;

	FallDownPlayer(int index);

};