#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"
#include <glm/glm.hpp>

class BugSwatPlayer : public ScorePlayer
{
public:
	BugSwatPlayer(int index);

	glm::vec2 position;
	double swatTime;
	bool swatting;

};