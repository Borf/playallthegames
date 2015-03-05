#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"
#include <glm/glm.hpp>
#include <blib/math/Rectangle.h>

class Car;

class RushHourPlayer : public AlivePlayer
{
public:
	RushHourPlayer(int index);


	glm::vec2 cursor;
	Car* followingCar;

	blib::math::Rectangle screenRect;

};