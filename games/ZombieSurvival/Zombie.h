#pragma once

#include <glm/glm.hpp>
class ZombieSurvivalPlayer;

namespace blib { class Animation; };

class Zombie
{
public:
	blib::Animation* zombieSprite;
	glm::vec2 position;
	float direction;
	
	ZombieSurvivalPlayer* target;


	Zombie();
	~Zombie();
};

