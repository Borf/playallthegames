#include "SkipRopePlayer.h"

SkipRopePlayer::SkipRopePlayer( int index ) : AlivePlayer(index)
{
	
	position = glm::vec2(600 +  ((index+1)/2) * 64 * (index%2==0 ? -1 : 1), 770);
	speed = 0;
}

