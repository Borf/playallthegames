#include "Enemy.h"

#include "PacManLevel.h"

namespace pacman
{

	Enemy::Enemy()
	{
		offset = 0;
	}

	glm::ivec2 Enemy::point()
	{
		return glm::ivec2(position.x, position.y);
	}

	void Enemy::update(PacManLevel* level, std::vector<PacManPlayer*> players, std::vector<Enemy*> enemies, float gameTime, float elapsedTime )
	{
		float distanceLeft = elapsedTime*6; // speed!
		while(distanceLeft > 0)
		{
			if(direction != glm::vec2(0,0))
			{
				offset += distanceLeft;
				if(offset > 1)
				{
					distanceLeft = offset-1;
					position+=direction;
					offset=0;
					direction = pickDirection(level, players, enemies, gameTime);
					if (inPenStatus == 0 && !level->inPen((int)(position.x+direction.x), (int)(position.y+direction.y)))
						inPenStatus = 1;

				}
				else
					distanceLeft = 0;
			}
			else
				distanceLeft = 0;
		}

	}

	std::vector<glm::vec2> Enemy::getOptions( PacManLevel* level, std::vector<Enemy*> enemies, float gameTime )
	{
		std::vector<glm::vec2> possible;
		if(level->isWalkable((int)position.x+1, (int)position.y, enemies, gameTime, this))
			possible.push_back(glm::vec2(1,0));
		if(level->isWalkable((int)position.x-1, (int)position.y, enemies, gameTime, this))
			possible.push_back(glm::vec2(-1,0));
		if(level->isWalkable((int)position.x, (int)position.y+1, enemies, gameTime, this))
			possible.push_back(glm::vec2(0,1));
		if(level->isWalkable((int)position.x, (int)position.y-1, enemies, gameTime, this))
			possible.push_back(glm::vec2(0,-1));

		return possible;
	}

	glm::vec2 Enemy::pickDirection( PacManLevel* level, std::vector<PacManPlayer*> players, std::vector<Enemy*> enemies, float gameTime )
	{
		std::vector<glm::vec2> options = getOptions(level, enemies, gameTime);

		if(options.size() == 0)
			return glm::vec2(0,0);
		if(options.size() == 1)
			return options[0];

		int r = rand() % options.size();
		while(options[r] == -direction)
			r = rand() % options.size();
		return options[r];
	}

}