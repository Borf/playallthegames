#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"

#include <glm/glm.hpp>

class MazeEscapePlayer : public Player
{
public:
	glm::vec2 position;
	float rotation;

	MazeEscapePlayer(int index);

};