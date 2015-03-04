#include "BulletHellPlayer.h"

#include <glm/gtc/matrix_transform.hpp>


BulletHellPlayer::BulletHellPlayer(int index) : AlivePlayer(index)
{
	rotation = 0;
}



glm::mat4 BulletHellPlayer::getMatrix()
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, glm::vec3(position, 0));
	matrix = glm::rotate(matrix, rotation, glm::vec3(0,0,1));
	return matrix;
}