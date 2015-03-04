#include "BattleLevel.h"

#include <blib/SpriteSheet.h>
#include <blib/SpriteBatch.h>

#include <glm/gtc/matrix_transform.hpp>

BattleLevel::BattleLevel( blib::SpriteSheet* texture )
{
	this->texture = texture;
}

void BattleLevel::draw( blib::SpriteBatch* spriteBatch )
{
	for(size_t y = 0; y < levelData.size(); y++)
	{
		for(size_t x = 0; x < levelData[y].size(); x++)
		{
			glm::mat4 matrix;
			matrix = glm::translate(matrix, glm::vec3(x*64, y*64, 0));
			if(levelData[y][x] == 0 && (x+y)%2 == 0)
			{
				matrix = glm::scale(matrix, glm::vec3(-1,-1,1));
				matrix = glm::translate(matrix, glm::vec3(-64,-64,0));
			}

			spriteBatch->draw(texture->getFrame(levelData[y][x], 0), matrix);
		}
	}
}

