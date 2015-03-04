#include "FlyFarPlayer.h"



FlyFarPlayer::FlyFarPlayer(int index) : AlivePlayer(index)
{
	position = glm::vec2(0, -50);
	rotation = 0;
	speed = 5;
}
