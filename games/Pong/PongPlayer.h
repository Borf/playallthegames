#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"

#include <glm/glm.hpp>

class PongPlayer : public ScorePlayer
{
public:
	glm::vec2 position;
	float rotation;

	PongPlayer(int index);

};