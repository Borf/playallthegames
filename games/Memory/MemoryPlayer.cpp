#include "MemoryPlayer.h"
#include "../../PlayAllTheGames/Settings.h"


MemoryPlayer::MemoryPlayer( int index ) : ScorePlayer(index)
{
	sel = glm::ivec2(-1, -1);
	sel2 = glm::ivec2(-1,-1);
	selected = false;
	selected2 = false;
	pressedButton = false;
}

void MemoryPlayer::setStart( Settings* settings )
{
	cursor = glm::vec2(((index == 0 || index == 2) ? ((settings->resX-settings->resY)/2+50) : settings->resX - ((settings->resX-settings->resY)/2+50)), (index  == 0 || index == 1) ? 50 : settings->resY-50);
	homePos = glm::vec2(((index == 0 || index == 2) ? 50 : settings->resX - (settings->resX-settings->resY)/2+50), (index == 0 || index == 1) ? 50 : settings->resY/2+50);
}

