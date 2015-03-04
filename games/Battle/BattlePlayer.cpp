#include "BattlePlayer.h"

#include "BattleEnemy.h"


BattlePlayer::BattlePlayer( int index ) : ScorePlayer(index)
{
	position.x = (double)(128 + 256 * index);
	position.y = 15*64;
	speed = glm::dvec2(0,0);
	upcount = 0;
	alive = true;
}

void BattlePlayer::hit( BattleCharacter* otherPlayer )
{
	alive = false;
	respawnTime = 4;
	score -= 2;
	if (dynamic_cast<BattlePlayer*>(otherPlayer) != NULL)
		dynamic_cast<BattlePlayer*>(otherPlayer)->score += 3;
	else if (dynamic_cast<BattleEnemy*>(otherPlayer))
		if (dynamic_cast<BattleEnemy*>(otherPlayer)->taggedPlayer != NULL)
			dynamic_cast<BattleEnemy*>(otherPlayer)->taggedPlayer->score += 3;
}

