#include "gameobject.h"
#include <blib/SpriteBatch.h>
#include <blib/Animation.h>
#include <blib/Util.h>

#include <Box2D/Box2D.h>
#include <glm/gtc/matrix_transform.hpp>
#include "levelobject.h"


PhysicsGameObject::PhysicsGameObject()
{
	body = NULL;
	flipHorizontal = false;
}

void SpriteGameObject::draw( const glm::mat4 &matrix, blib::SpriteBatch* spriteBatch )
{
	spriteBatch->draw(sprite, matrix, sprite->center);
}

void InvisibleGameObject::draw( const glm::mat4 &matrix, blib::SpriteBatch* spriteBatch )
{
}

glm::mat4 PhysicsGameObject::getMatrix(const LevelObjectBase* object) const
{
	glm::mat4 matrix;
	if(body)
	{
		matrix = glm::translate(matrix, glm::vec3(body->GetPosition().x*128, body->GetPosition().y*128, 1));
		matrix = glm::rotate(matrix, glm::degrees(body->GetAngle()), glm::vec3(0,0,1));
		if(flipHorizontal)
		{
			float playerAngle = blib::util::wrapAngleRad(body->GetAngle() + glm::radians(90.0f));
			bool flip = playerAngle < 0;
			if(flip)
				matrix = glm::scale(matrix, glm::vec3(1,-1,1));
		}
	}
	else
	{
		matrix = glm::translate(matrix, glm::vec3(object->position + object->size/2.0f,0));
	}
	return matrix;
}

void AnimationGameObject::draw(const glm::mat4 &matrix, blib::SpriteBatch* spriteBatch)
{
	animation->draw(*spriteBatch, matrix);
}