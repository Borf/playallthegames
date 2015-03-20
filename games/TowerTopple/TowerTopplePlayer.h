#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"

#include <glm/glm.hpp>
#include <blib/math/Ray.h>

class btRigidBody;

class TowerTopplePlayer : public ScorePlayer
{
public:
	TowerTopplePlayer(int index);
	blib::math::Ray shootRay;

	glm::vec2 cursor;

	bool bla = false;

	float ballTImer;


	std::vector<btRigidBody*> balls;


};