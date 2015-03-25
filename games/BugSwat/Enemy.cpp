#include "Enemy.h"
#include "../../PlayAllTheGames/Settings.h"
#include <blib/Util.h>

#include <glm/gtc/matrix_transform.hpp>
namespace bugswat
{
	Enemy::Enemy()
	{
		alive = true;
	}

	bool Enemy::onScreen(Settings* settings)
	{
		return position.x > -10 && position.x < 1920 + 10 && position.y > -10 && position.y < 1080 + 10;
	}

	void Enemy::update(float elapsedTime)
	{
		if (alive)
			position += elapsedTime * speed * blib::util::fromAngle(rotation);
		else
		{
			rotation += 0.2f * elapsedTime;
			position.x += (float)glm::sin(position.y) * elapsedTime;
			position.y += 10 * elapsedTime;
		}
	}

	glm::mat4 Enemy::getMatrix()
	{
		glm::mat4 matrix;
		matrix = glm::translate(matrix, glm::vec3(position, 0));
		matrix = glm::rotate(matrix, glm::degrees(rotation), glm::vec3(0, 0, 1));
		return matrix;
	}

}