#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

#include <glm/glm.hpp>

class JumpManPlayer : public AlivePlayer
{
public:
	JumpManPlayer(int index);
	glm::vec2 position;
	int location;
	float jumpSpeed;

};