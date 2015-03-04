#pragma once

#include <glm/glm.hpp>
#include <blib/TextureMap.h>

class b2Body;
class LevelObjectBase;
namespace blib { class Texture; class SpriteBatch; class Animation; }

template<class drawtype, class objtype>
class GameObject : public drawtype, public objtype
{
public:
	virtual void draw(LevelObjectBase* object, blib::SpriteBatch* spriteBatch)
	{
		drawtype::draw(objtype::getMatrix(object), spriteBatch);
	}
};


class PhysicsGameObject
{
protected:
	bool flipHorizontal;
public:
	PhysicsGameObject();
	b2Body* body;
	virtual glm::mat4 getMatrix(const LevelObjectBase* object) const;
};


class SpriteGameObject
{
public:
	blib::TextureMap::TexInfo* sprite;
	virtual void draw(const glm::mat4 &matrix, blib::SpriteBatch* spriteBatch);
};

class InvisibleGameObject
{
public:
	virtual void draw(const glm::mat4 &matrix, blib::SpriteBatch* spriteBatch);
};


class AnimationGameObject
{
public:
	blib::Animation* animation;
	virtual void draw(const glm::mat4 &matrix, blib::SpriteBatch* spriteBatch);
};