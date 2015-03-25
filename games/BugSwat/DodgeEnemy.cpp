#include "DodgeEnemy.h"
#include "BugSwat.h"

namespace bugswat
{
	DodgeEnemy::DodgeEnemy(BugSwat* bugSwat)
	{
		this->bugSwat = bugSwat;
	}

	void DodgeEnemy::update(float elapsedTime)
	{
		Enemy::update(elapsedTime);
		BugSwatPlayer* closestPlayer = blib::linq::min<float, BugSwatPlayer*>(bugSwat->players, [this](BugSwatPlayer* p) { return glm::length(p->position - position); }, [](BugSwatPlayer* p) { return p; });

		float targetRotation = rotation;

		if (glm::length(closestPlayer->position - position) < 100)
		{
			targetRotation = atan2(position.y - closestPlayer->position.y, position.x - closestPlayer->position.x);
		}
		rotation = targetRotation;
	}


}