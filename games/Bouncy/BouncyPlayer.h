#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

#include <glm/glm.hpp>

class BouncyPlayer : public AlivePlayer
{
public:
	BouncyPlayer(int index);

	glm::vec2 position;
	glm::vec2 speed;

	glm::mat4 getMatrix();

};
