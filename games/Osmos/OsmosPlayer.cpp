#include "OsmosPlayer.h"

#include "Sphere.h"

#define _USE_MATH_DEFINES
#include <math.h>

OsmosPlayer::OsmosPlayer( int index ) : Player(index)
{

}



void OsmosPlayer::testCollision( std::vector<Sphere*> &toTest )
{
	for (auto p : toTest)
	{
		if (sphere == *p || p->size <= 0)
			continue;

		glm::vec2 diff = sphere.position - p->position;
		float dist = sphere.size / 2 + p->size / 2;
		if (glm::length(diff) < dist)
		{
			float lenDiff = dist - glm::length(diff);

			double mass1 = sphere.getMass();
			double mass2 = p->getMass();

			if (mass1 > mass2)
			{
				float bla = (p->size / 2) - lenDiff;
				double mass2_new = 0;
				if (bla > 0)
					mass2_new = glm::max(0.0, M_PI * bla * bla);
				mass1 += (mass2 - mass2_new);
				mass2 = mass2_new;

				sphere.oldPosition += 0.5f * (sphere.position - sphere.oldPosition) * (float)(500 / (mass1 - mass2));

			}
			else
			{
				float bla = (sphere.size / 2) - lenDiff;
				double mass1_new = 0;
				if (bla > 0)
					mass1_new = glm::max(0.0, M_PI * bla * bla);
				mass2 += (mass1 - mass1_new);
				mass1 = mass1_new;
				p->oldPosition += 0.5f * (p->position - p->oldPosition) * (float)(500 / (mass2 - mass1));
			}

			sphere.setMass((float)mass1);
			p->setMass((float)mass2);
		}

	}
}

