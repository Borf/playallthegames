#pragma once

#include "gameobject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Box2D/Box2D.h>
#include <string>
#include <list>
#include <map>

#include <blib/TextureMap.h>

class b2Shape;
class b2World;
class b2Joint;
class b2DistanceJoint;
class Level;

namespace blib { class SpriteBatch; };

class LevelObjectBase
{
public:
	std::string type;
	std::string name;
	glm::vec2 position;
	glm::vec2 size;
	std::map<std::string, std::string> stringValues;
	std::map<std::string, float> floatValues;

	//optional
	blib::TextureMap::TexInfo* texture;
	blib::TextureMap::TexInfo* texture1;
	blib::TextureMap::TexInfo* texture2;

	bool triggered;

	LevelObjectBase()
	{
		texture1 = NULL;
		texture2 = NULL;
	}
	virtual glm::vec2 getCenter();

	bool contains(glm::vec2 point);

	bool hasFloatValue(std::string name);
	bool hasStringValue(std::string name);

	virtual void draw(blib::SpriteBatch* spriteBatch) = 0;
	virtual glm::mat4 getMatrix() const = 0;

	virtual void init(Level* level, b2World* world) = 0;
	std::list<b2Shape*> buildShape( const std::string &shapeName, const std::string &filename = "");
	virtual void cleanWorld(b2World* world) = 0;
	virtual void doAction(std::string action);
};

template <class animationtype>
class LevelObject : public LevelObjectBase, public GameObject<animationtype, PhysicsGameObject>
{
private:
	void myInit(Level* level)
	{//method is specialized down below
	}
public:
	virtual void init(Level* level, b2World* world);
	void draw(blib::SpriteBatch* spriteBatch) { GameObject<animationtype, PhysicsGameObject>::draw(this, spriteBatch); };
	virtual glm::mat4 getMatrix() const
	{ 
	//method is specialized down below
		return glm::mat4();
	};
	glm::vec2 getCenter()
	{
		return PhysicsGameObject::body->GetWorldCenter();
	}

	void cleanWorld(b2World* world)
	{
		if(PhysicsGameObject::body)
			world->DestroyBody(PhysicsGameObject::body);
		PhysicsGameObject::body = NULL;
	}
};




//template <>
//glm::mat4 LevelObject<SpriteGameObject>::getMatrix() const;// { return glm::scale(GameObject<SpriteGameObject, PhysicsGameObject>::getMatrix(), glm::vec3(size.x / sprite->width, size.y / sprite->height, 1.0f)); }

//template <>
//glm::mat4 LevelObject<InvisibleGameObject>::getMatrix() const;// { return GameObject<InvisibleGameObject, PhysicsGameObject>::getMatrix(); };

//template <>
//glm::mat4 LevelObject<AnimationGameObject>::getMatrix() const;// { return GameObject<AnimationGameObject, PhysicsGameObject>::getMatrix(); };


class ButtonLevelObject : public LevelObject<SpriteGameObject>
{
public:
	b2Body* anchor;
	b2DistanceJoint* joint1;
	b2Joint* joint2;

	float lastTriggerTime;

	virtual void init(Level* level, b2World* world);
	virtual void cleanWorld(b2World* world);
};



