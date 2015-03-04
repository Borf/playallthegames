#include "BattleCharacter.h"
#include "BattleLevel.h"
#include "BattleEnemy.h"
#include "BattlePlayer.h"


bool BattleCharacter::hasCollision( BattleLevel* level )
{
	int y1 = (int)glm::floor(position.y / 64);
	int y2 = (int)glm::ceil(position.y / 64);

	int x1 = (int)glm::floor(position.x / 64);
	int x2 = (int)glm::ceil(position.x / 64);

	if (x2 >= (int)level->levelData[0].size())
		x2 = 0;
	if (x1 < 0)
		x1 += (int)level->levelData[0].size()-1;

	if (y1 >= 0 && level->levelData[y1][x1] != 0)
		return true;
	if (y1 >= 0 && level->levelData[y1][x2] != 0)
		return true;

	if (y2 >= 0 && level->levelData[y2][x1] != 0)
		return true;
	if (y2 >= 0 && level->levelData[y2][x2] != 0)
		return true;
	return false;
}

bool BattleCharacter::isOnBlock( BattleLevel* level )
{
	int y2 = (int)glm::ceil((position.y + 1) / 64);
	int x1 = (int)glm::floor(position.x / 64);
	int x2 = (int)glm::ceil(position.x / 64);

	if (x2 >= (int)level->levelData[0].size())
		x2 = 0;
	if (x1 < 0)
		x1 += (int)level->levelData[0].size();


	if (y2 >= 0 && level->levelData[y2][x1] != 0)
		return true;
	if (y2 >= 0 && level->levelData[y2][x2] != 0)
		return true;
	return false;
}

void BattleCharacter::updateMovement( BattleLevel* level, const std::vector<BattleCharacter*> &players, float elapsedTime )
{
	if (!alive)
		return;

	BattleEnemy* enemy = dynamic_cast<BattleEnemy*>(this);

	if (enemy == NULL || !enemy->upsidedown)
	{
		if (isOnBlock(level))
		{
			speed.x *= 0.7f;
			speed.x += 4*directionalStick.x;
			speed.x = glm::clamp(speed.x, -8.0, 8.0);
		}
		else
		{
			speed.x *= 0.9f;
			speed.x += 0.9f*directionalStick.x;
			speed.x = glm::clamp(speed.x, -8.0, 8.0);
		}
	}

	double beforeX = position.x;
	position.x += speed.x * elapsedTime * 60;
	if (position.x < -64)
		position.x += (1920 + 64);
	if (position.x > 1920)
		position.x -= (1920 + 64);


	if (hasCollision(level))
	{
		position.x = beforeX;
		speed.x = 0;
	}
	for (auto pp : players)
	{
		if (pp == this || !pp->alive)
			continue;
		if (glm::abs(pp->position.x - position.x) < 64 && position.y > pp->position.y - 64 && position.y < pp->position.y + 64/*player jumped on height*/)
		{//collision between p and pp;
			position.x = beforeX;
			//if still colliding, then one is probably jumping on the other
			if (glm::abs(pp->position.x - position.x) < 64 && position.y > pp->position.y - 64 && position.y < pp->position.y + 64)
			{
				if (position.y < pp->position.y)
				{
					position.y -= speed.y*elapsedTime*60;
					speed.y = -14;
					position.y += speed.y*elapsedTime*60;
					if (pp->isOnBlock(level))
						upcount = 1;
					BattlePlayer* player = dynamic_cast<BattlePlayer*>(this);
					BattleEnemy* otherEnemy = dynamic_cast<BattleEnemy*>(pp);
					pp->speed.y = 14;
					pp->hit(this);
				}
				else
				{
					pp->position.y -= pp->speed.y*elapsedTime*60;
					pp->speed.y = -14;
					pp->position.y += pp->speed.y*elapsedTime*60;
					if (isOnBlock(level))
						pp->upcount = 1;
					BattlePlayer* player = dynamic_cast<BattlePlayer*>(pp);
					BattleEnemy* otherEnemy = dynamic_cast<BattleEnemy*>(this);
					speed.y = 14;
					hit(pp);
				}
			}
			else
			{//horizontal collision. If one of 2 is a player, he should die. If one is an upsidedown enemy, he should be shot
				if (dynamic_cast<BattleEnemy*>(pp) != NULL)
				{
					if (dynamic_cast<BattleEnemy*>(pp)->upsidedown)
					{
						if (pp->speed.x == 0)
						{
							pp->speed.x = 10;
							if (position.x > pp->position.x)
								pp->speed.x = -10;
							pp->position.x += pp->speed.x*elapsedTime*60;
						}
						else
						{
							hit(pp);
							pp->speed.x = -pp->speed.x;
							pp->position.x += pp->speed.x*elapsedTime*60;
						}
					}
					else
					{
						if (dynamic_cast<BattlePlayer*>(this) != NULL)
							hit(pp);
						speed.x = -speed.x;
						directionalStick = glm::vec2(-directionalStick.x, directionalStick.y);
					}
				}
				else if (dynamic_cast<BattleEnemy*>(this) != NULL)
				{
					if (dynamic_cast<BattleEnemy*>(this)->upsidedown)
					{
						if (speed.x == 0)
						{
							speed.x = 10;
							if (pp->position.x > position.x)
								speed.x = -10;
							position.x += pp->speed.x*elapsedTime*60;
						}
						else
						{
							pp->hit(this);
							speed.x = -speed.x;
							position.x += speed.x*elapsedTime*60;
						}
					}
					else
					{
						if (dynamic_cast<BattlePlayer*>(pp))
							pp->hit(this);
						pp->speed.x = -pp->speed.x;
						pp->directionalStick = glm::vec2(-pp->directionalStick.x, pp->directionalStick.y);
					}
				}
				else
				{
					speed.x = 0;
					pp->speed.x = 0;
					pp->directionalStick = glm::vec2(-pp->directionalStick.x, pp->directionalStick.y);
				}
			}
		}
	}

	if (wantsToJump)
	{
		printf("%f, %f\n", speed.y, upcount);
		printf("On block: %s\n", isOnBlock(level) ? "yes" : "no");
	}

	if (wantsToJump && (isOnBlock(level) || (upcount > 0 && upcount < 14)) && !hasCollision(level))
	{
		speed.y = -14;
		upcount+=elapsedTime*60;
	}
	else if (!wantsToJump)
		upcount = 0;

	bool onBlockBefore = hasCollision(level);
	position.y += speed.y*elapsedTime*60;

	if (speed.y < 0 && hasCollision(level) && !onBlockBefore) // ceiling collision
	{
		position.y -= speed.y*elapsedTime*60;
		speed.y = 0.5f * -speed.y;
	}


	speed.y += 1*elapsedTime*60;

	if (isOnBlock(level))
	{
		position.y = 64 * glm::round(position.y / 64);
		speed.y = 0;
	}
}

