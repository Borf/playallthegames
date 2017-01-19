#include "CheckersPlayer.h"
#include <blib/Math.h>

namespace checkers
{
	CheckersPlayer::CheckersPlayer(int index) : ScorePlayer(index)
	{
		for (int y = 2; y < 3; y++)
			for (int x = 0; x < 4; x++)
				if((x+y)%2 == 0)
					pieces.push_back(glm::ivec4(x,y,index,0));
	}
}