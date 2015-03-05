#pragma once


#include "../../PlayAllTheGames/ScorePlayer.h"
#include "BattleCharacter.h"


class BattlePlayer : public ScorePlayer, public BattleCharacter
{
public:
	BattlePlayer(int index);

	virtual void hit( BattleCharacter* otherPlayer );

};