#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

class BulletHellPlayer : public AlivePlayer
{
public:
	BulletHellPlayer(int index);
	glm::vec2 position;
	float rotation;


	glm::mat4 getMatrix();
};