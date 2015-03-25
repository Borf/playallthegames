#include <blib/linq.h>
#include "FollowGhost.h"

#include "PacManPlayer.h"

namespace pacman
{
	FollowGhost::FollowGhost( glm::vec2 position, glm::vec2 direction, glm::vec4 color, int penStatus )
	{
		this->position = position;
		this->direction = direction;
		this->color = color;
		this->inPenStatus = penStatus;
	}

	glm::vec2 FollowGhost::pickDirection( PacManLevel* level, std::vector<PacManPlayer*> players, std::vector<Enemy*> enemies, float gameTime )
	{
		if(blib::linq::any(players, [] (PacManPlayer* p) { return p->alive; }))
		{
			PacManPlayer* closestPlayer = blib::linq::min<float, PacManPlayer*>(players, [this] (PacManPlayer* p) { return p->alive ? glm::length(p->position - position) : 9999999; }, [] (PacManPlayer* p) { return p; });
	
			glm::vec2 preferredDirection;
			if (closestPlayer != NULL)
			{
				if (closestPlayer->position.x < position.x)
					preferredDirection = glm::vec2(-1, 0);
				if (closestPlayer->position.x > position.x)
					preferredDirection = glm::vec2(1, 0);
				if (closestPlayer->position.y < position.y)
					preferredDirection = glm::vec2(0, -1);
				if (closestPlayer->position.y > position.y)
					preferredDirection = glm::vec2(0, 1);
			}

			std::vector<glm::vec2> possible = getOptions(level, enemies, gameTime);

			if(preferredDirection != -direction && blib::linq::contains(possible, [preferredDirection] (glm::vec2 p) { return p == preferredDirection; }))
				return preferredDirection;
		}
		return Enemy::pickDirection(level, players, enemies, gameTime);

	}

}