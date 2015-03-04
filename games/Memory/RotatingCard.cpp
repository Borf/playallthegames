#include "RotatingCard.h"


RotatingCard::RotatingCard( glm::ivec2 image, glm::vec2 pos, glm::vec2 targetPos )
{
	this->image = image;
	this->pos = pos;
	this->targetPos = targetPos;
	this->rotation = 0;
}



void RotatingCard::update(float elapsedTime)
{
	glm::vec2 diff = targetPos - pos;
	float dl = glm::length(diff);

	bool rotate = dl > 15 && rotation > 0.2;

	if(dl > 15)
	{
		pos += diff/dl* 15.0f * elapsedTime * 60.0f;
		rotation += 0.15f * elapsedTime * 60.0f;
	}
	else if (rotate)
		rotation += 0.15f * elapsedTime * 60.0f;
	else
	{
		pos = targetPos;
		rotation = 0;
	}
}

