#include "../../playallthegames/ScorePlayer.h"

namespace nipplerub
{
	class Player : public ScorePlayer
	{
	public:
		Player(int index) : ScorePlayer(index) 
		{
			for (int i = 0; i < 2; i++)
			{
				scores[i] = 0;
				counter[i] = 0;
			}
		}

		glm::vec2 lastPos[2];
		int scores[2];
		float counter[2];
	};


}