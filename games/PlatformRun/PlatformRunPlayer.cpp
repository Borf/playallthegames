#include "PlatformRunPlayer.h"


PlatformRunPlayer::PlatformRunPlayer( int index ) : AlivePlayer(index)
{
	position = glm::vec2(1700 - 170 * index, 128 + rand() % 400);
}

blib::math::Rectangle PlatformRunPlayer::boundingbox()
{
	return blib::math::Rectangle(position, 64,64);
}

