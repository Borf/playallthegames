#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"

#include <glm/glm.hpp>
#include <blib/math/Ray.h>

class TowerTopplePlayer : public ScorePlayer
{
public:
	TowerTopplePlayer(int index);
	blib::math::Ray shootRay;


};