#include "Bullet.h"

#include "../../PlayAllTheGames/Settings.h"

#include <glm/gtc/matrix_transform.hpp>


Bullet::Bullet(glm::vec2 pos, glm::vec2 dir)
{
	color = glm::vec4(1,1,1,1);
	this->pos = pos;
	this->dir = dir;
}

void Bullet::update( float elapsedTime, Settings* settings)
{
	pos += dir * settings->scale * elapsedTime;
	if (pos.y > settings->resY || pos.y < 0)
		dir.y = -dir.y;
}


glm::mat4 Bullet::getMatrix()
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, glm::vec3(pos, 0));
	return matrix;
}