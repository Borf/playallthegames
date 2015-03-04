#include "RushHourPlayer.h"

RushHourPlayer::RushHourPlayer(int index) : AlivePlayer(index), screenRect(0,0,0,0)
{
	alive = true;
	followingCar = NULL;

	cursor.x = 550.0f + (index % 2) * 800;
	cursor.y = 300.0f + (index / 2) * 500;


	screenRect.topleft.x = (float)((index % 2) * 1920 / 2);
	screenRect.topleft.y = (float)((index / 2) * 1080 / 2);

	screenRect.bottomright = screenRect.topleft + glm::vec2(1920 / 2, 1080 / 2);


}

