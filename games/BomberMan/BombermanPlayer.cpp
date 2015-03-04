#include "BombermanPlayer.h"


BomberManPlayer::BomberManPlayer( int index ) : AlivePlayer(index)
{
	speed = 5;
	bombs = 1;
	flameLength = 3;
	pushBombs = false;
}

