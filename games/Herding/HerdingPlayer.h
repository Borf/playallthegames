#pragma once

#include "../../ScorePlayer.h"

class HerdingPlayer : public ScorePlayer
{
public:
	HerdingPlayer(int index);
	glm::vec2 position;
	glm::vec2 basePosition;
};