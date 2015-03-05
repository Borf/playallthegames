#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

class BomberManPlayer : public AlivePlayer
{
public:
	BomberManPlayer(int index);

	glm::vec2 position;
	int speed;
	int bombs;
	int flameLength;
	bool pushBombs;
};