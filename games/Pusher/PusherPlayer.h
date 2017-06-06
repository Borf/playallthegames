#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"
#include <glm/glm.hpp>

class PusherPlayer : public AlivePlayer
{
public:
    glm::vec2 position;

    float rotation = 0.0f;

    PusherPlayer(int index);
	blib::math::Rectangle hitbox();
};
