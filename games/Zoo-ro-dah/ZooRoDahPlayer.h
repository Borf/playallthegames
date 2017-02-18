#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"

#include <glm/glm.hpp>

namespace zoorodah
{

	class ZooRoDahPlayer : public ScorePlayer
	{
	public:
		glm::vec2 position;
		float rotation;

		ZooRoDahPlayer(int index);

	};
}