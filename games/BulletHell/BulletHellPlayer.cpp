#include "BulletHellPlayer.h"

#include <blib/audio/AudioManager.h>
#include <glm/gtc/matrix_transform.hpp>


BulletHellPlayer::BulletHellPlayer(int index) : AlivePlayer(index)
{
	rotation = 0;
	moveSound = blib::AudioManager::getInstance()->loadSample("assets/games/BulletHell/spaceship.wav");
	moveSound->setVolume(0);
	moveSound->play(true);
}


BulletHellPlayer::~BulletHellPlayer()
{
	moveSound->stop();
	delete moveSound;
}


glm::mat4 BulletHellPlayer::getMatrix()
{
	glm::mat4 matrix;
	matrix = glm::translate(matrix, glm::vec3(position, 0));
	matrix = glm::rotate(matrix, rotation, glm::vec3(0,0,1));
	return matrix;
}