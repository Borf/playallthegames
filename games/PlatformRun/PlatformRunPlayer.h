#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

#include <glm/glm.hpp>

class PlatformRunPlayer : public AlivePlayer
{
public:
	glm::vec2 position;
	glm::vec2 speed;

	PlatformRunPlayer(int index);


	blib::math::Rectangle boundingbox();

};