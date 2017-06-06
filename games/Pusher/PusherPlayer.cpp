#include "PusherPlayer.h"
#include <blib/math.h>

PusherPlayer::PusherPlayer(int index) : AlivePlayer(index)
{
    position = glm::vec2(1920 / 2 + blib::math::randomFloat(0, 1), 1000 + blib::math::randomFloat(0, 0));
}

blib::math::Rectangle PusherPlayer::hitbox()
{
	return blib::math::Rectangle(position, 64, 64);
}