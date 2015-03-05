#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

class TronPlayer : public AlivePlayer
{
public:
	TronPlayer(int index);

	glm::vec2 position;
	glm::vec2 prevPosition;
	glm::vec2 direction;

};