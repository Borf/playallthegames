#pragma once

#include <glm/glm.hpp>

#include <vector>

class BattleLevel;

class BattleCharacter
{
public:
	glm::dvec2 position;
	glm::dvec2 speed;
	double upcount;
	bool alive;
	double respawnTime;

	glm::dvec2 directionalStick;
	bool wantsToJump;
	virtual void hit(BattleCharacter* otherPlayer) = 0;
	virtual bool hasCollision(BattleLevel* level);
	virtual bool isOnBlock(BattleLevel* level);
	virtual void updateMovement(BattleLevel* level, const std::vector<BattleCharacter*> &players, float elapsedTime);

};