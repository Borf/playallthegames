#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"
#include <glm/glm.hpp>

namespace blib { class Animation; }

class ZombieSurvivalPlayer : public AlivePlayer
{
public:
	glm::vec2 position;
	float rotation;
	float accuracy;
	blib::Animation* playerAnimation;

	ZombieSurvivalPlayer(int index);

};