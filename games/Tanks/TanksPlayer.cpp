#include "TanksPlayer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Box2D/Box2D.h>

#include "../../PlayAllTheGames/Settings.h"

#define SCALE 50.0f

TanksPlayer::TanksPlayer( int index ) : AlivePlayer(index)
{
	targetAngle = 0;
	lastShootTime = 0;
}

glm::mat4 TanksPlayer::getMatrix(Settings* settings)
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, 100.0f * glm::vec3(body->GetPosition().x, body->GetPosition().y, 0) + glm::vec3(0, -10,0 ));
//	matrix = glm::rotate(matrix, glm::degrees(body->GetAngle())+90, glm::vec3(0,0,1));
	matrix = glm::scale(matrix, glm::vec3(1.5f, 1.5f, 1.0f));
	return matrix;
}

glm::mat4 TanksPlayer::getTurretMatrix( Settings* settings )
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, 100.0f * glm::vec3(body->GetPosition().x, body->GetPosition().y, 0));
//	matrix = glm::rotate(matrix, glm::degrees(turret->GetAngle())+90, glm::vec3(0,0,1));
	return matrix;
}

