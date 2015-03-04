#pragma once

#include <string>

#include "gameobject.h"

#include <blib/SpriteBatch.h>
#include <blib/TextureMap.h>

class b2World;
class b2Body;
class b2Shape;
class b2Joint;
class b2DistanceJoint;
class Level;
class Enemy;
class PewPewPlayer;
namespace blib { class SpriteBatch; class TextureMap; }


class LevelObjectBase;

template <class T> class LevelObject;


class DecorObject
{
public:
	glm::vec2 position;
	glm::vec2 size;
	std::string filename;
	blib::TextureMap::TexInfo* texInfo;
	int rotation;
	bool fliph;
	bool flipv;
};



class Level
{
	blib::TextureMap* textureMap;

	blib::TextureMap::TexInfo* errorTexture;

	blib::SpriteBatch::Cache* levelCache;
	blib::SpriteBatch::Cache* levelOverlayCache;

	blib::TextureMap* objectTextureMap;
	std::vector<std::list<DecorObject*> > decorObjects;
	std::vector<LevelObjectBase*> objects;

public:

	b2Body* body;


	Level(std::string filename, b2World* world, blib::ResourceManager* resourceManager, blib::Renderer* renderer);
	~Level();

	void draw(blib::SpriteBatch* spriteBatch);
	void drawObjects(blib::SpriteBatch* spriteBatch);
	void drawOverlay(blib::SpriteBatch* spriteBatch);

	void update(float gameTime, float elapsedTime, const std::vector<PewPewPlayer*> &players);
	void cleanWorld(b2World* world);

	void triggerActions(std::string objects, std::string actions);



	std::map<std::string, std::vector<LevelObjectBase*> > objectCache;
	LevelObjectBase* getObject(std::string name);
	std::vector<LevelObjectBase*> getObjects(std::string name);
	void getTouchingBodies(b2Body* body, std::vector<b2Body*> &touchingBodies);

	std::vector<Enemy*> getEnemies() const;

	friend class LevelObject < SpriteGameObject > ;
};
