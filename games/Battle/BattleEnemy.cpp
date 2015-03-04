#include "BattleEnemy.h"
#include "BattlePlayer.h"


BattleEnemy::BattleEnemy(glm::vec2 position, glm::vec2 stick)
{
	upcount = 0;
	alive = true;
	upsidedown = false;
	taggedPlayer = NULL;
	wantsToJump = false;

	this->position = position;
	this->directionalStick = stick;
}


void BattleEnemy::hit( BattleCharacter* otherPlayer )
{
	if (!upsidedown)
	{
		taggedPlayer = dynamic_cast<BattlePlayer*>(otherPlayer);
		upsidedown = true;
		speed.x = 0;
	}
	else
	{
		alive = false;
		if (dynamic_cast<BattlePlayer*>(otherPlayer) != NULL)
			dynamic_cast<BattlePlayer*>(otherPlayer)->score += 1;
	}
}

