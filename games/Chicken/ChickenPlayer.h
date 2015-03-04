#pragma once

#include "../../AlivePlayer.h"

class ChickenPlayer : public AlivePlayer
{
public:
	ChickenPlayer(int index);
	glm::vec2 position;
	bool driving;
};