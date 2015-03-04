#pragma once

#include "BattleCharacter.h"

class BattlePlayer;

class BattleEnemy : public BattleCharacter
{
public:
	bool upsidedown;
	BattlePlayer* taggedPlayer;

	BattleEnemy(glm::vec2 position, glm::vec2 stick);

	void hit(BattleCharacter* otherPlayer);
};