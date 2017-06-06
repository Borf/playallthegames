#pragma once

#include "PusherPlayer.h"

class PusherEnemy
{

public:

    float *speed;

    glm::vec2 position;
    bool isComingFromLeft;

    PusherEnemy(glm::vec2 position, bool isComingFromLeftbool, float *speed);
    void update(float elapsedTime);
	blib::math::Rectangle hitbox();
};