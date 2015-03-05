#pragma once

#include "../../PlayAllTheGames/Player.h"

#include "Sphere.h"
#include <vector>

class OsmosPlayer : public Player
{
public:
	OsmosPlayer(int index);
	Sphere sphere;
	void testCollision(std::vector<Sphere*> &toTest);
};