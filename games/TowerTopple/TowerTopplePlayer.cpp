#include "TowerTopplePlayer.h"


TowerTopplePlayer::TowerTopplePlayer(int index) : ScorePlayer(index), shootRay(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0))
{
	ballTImer = 0;
}

