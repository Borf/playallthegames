#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"
#include <glm/glm.hpp>

#include <vector>

namespace blib { class Texture; };
class Car;

class MicroRacePlayer : public AlivePlayer
{
public:
	MicroRacePlayer(int index);

	float rot;
	glm::vec2 position;
	glm::vec2 targetPosition;
	bool moved;
	blib::Texture* sprite;

	void update(const std::vector<MicroRacePlayer*> &players, float gameTime, float speed, const std::vector<Car*> &otherCars);
	void handleControllerMovement(const std::vector<MicroRacePlayer*> &players);
	void testCarCollision( const std::vector<Car*> & otherCars );
};