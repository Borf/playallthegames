#include "BouncyPlayer.h"

#include "../../PlayAllTheGames/Settings.h"

#include <glm/gtc/matrix_transform.hpp>

BouncyPlayer::BouncyPlayer( int index ) : AlivePlayer(index)
{
	speed = glm::vec2(0,-12 + 24 * (((index+1)/2)%2));
	position = glm::vec2(960 + ((index+1)/2) * 100 * (index%2==0 ? -1 : 1), 540);
}


glm::mat4 BouncyPlayer::getMatrix()
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, glm::vec3(position,0));
	if(speed.y > 0)
	{

	}
	return matrix;
}
