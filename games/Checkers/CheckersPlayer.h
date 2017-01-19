#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"

#include <glm/glm.hpp>

namespace checkers
{
	class CheckersPlayer : public ScorePlayer
	{
	public:
		std::vector<glm::ivec4> pieces;

		glm::vec2 cursor;
		int pickedPiece = -1;


		CheckersPlayer(int index);

	};
}
