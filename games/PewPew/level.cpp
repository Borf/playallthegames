#include "level.h"
#include "CollisionProps.h"
#include "levelobject.h"
#include "collisionlayers.h"

#include "Butterfly.h"
#include "Carni.h"
#include "chargerbird.h"

#include <blib/Util.h>
#include <blib/util/FileSystem.h>
#include <blib/util/StreamReader.h>
#include <blib/TextureMap.h>
#include <blib/ResourceManager.h>
#include <blib/util/Log.h>
#include <blib/linq.h>

#include <glm/gtc/matrix_transform.hpp>
#include <Box2D/Box2D.h>

#include "PewPewPlayer.h"



using blib::util::Log;


std::string readString(blib::util::StreamIn* file)
{
	int len = file->readShort();
	char buf[1024];
	file->read(buf, len);
	return std::string(buf, len);
}


Level::Level(std::string filename, b2World* world, blib::ResourceManager* resourceManager, blib::Renderer* renderer)
{
	levelCache = NULL;
	levelOverlayCache = NULL;

	textureMap = resourceManager->getResource<blib::TextureMap>();
	objectTextureMap = resourceManager->getResource<blib::TextureMap>();

	errorTexture = textureMap->addTexture("assets/games/pewpew/textures/level/tiles/error.png");

	blib::util::StreamReaderFile* file = new blib::util::StreamReaderFile(blib::util::FileSystem::openRead(filename));

	std::vector<std::vector<glm::vec2> > polygons;
	int nPolygons = blib::util::swapByteOrder(file->readInt());
	for (int i = 0; i < nPolygons; i++)
	{
		std::vector<glm::vec2> polygon;
		int nVertices = blib::util::swapByteOrder(file->readInt());
		for (int ii = 0; ii < nVertices; ii++)
		{
			float x = blib::util::swapByteOrder(file->readFloat());
			float y = blib::util::swapByteOrder(file->readFloat());
			polygon.push_back(glm::vec2(x, y));
		}
		polygons.push_back(polygon);
	}

	int nObjects = blib::util::swapByteOrder(file->readInt());
	for (int i = 0; i < nObjects; i++)
	{
		std::string type = readString(file);
		LevelObjectBase* object = NULL;

		if (type == "anchor" || type == "timer" || type == "wind" || type == "start" || type == "vent" || type == "trigger") // wind should be a particle effect
			object = new LevelObject<InvisibleGameObject>();
		else if (type == "button")
			object = new ButtonLevelObject();
		else if (type == "enemy")
		{
			int index = file->tell();
			readString(file); // name
			file->seek(16, blib::util::StreamSeekable::CURRENT);
			int nStrings = blib::util::swapByteOrder(file->readInt());
			for (int ii = 0; ii < nStrings; ii++)
			{
				std::string key = readString(file);
				std::string value = readString(file);
				std::transform(key.begin(), key.end(), key.begin(), ::tolower);
				std::transform(value.begin(), value.end(), value.begin(), ::tolower);
				if (key == "type")
				{
					if (value == "bird")
						object = new ChargerBird(world, resourceManager, NULL);
					else if (value == "carni")
						object = new Carni(world, resourceManager, NULL);
					else if (value == "butterfly")
						object = new Butterfly(world, resourceManager, NULL);
					else
						Log::out << "Unknown enemy type: " << value << Log::newline;

					break;
				}
			}

			file->seek(index, blib::util::StreamSeekable::BEGIN);

		}
		else
			object = new LevelObject<SpriteGameObject>();
		object->type = type;

		object->name = readString(file);
		std::transform(object->name.begin(), object->name.end(), object->name.begin(), ::tolower);
		std::transform(object->type.begin(), object->type.end(), object->type.begin(), ::tolower);
		{
			float x = blib::util::swapByteOrder(file->readFloat());
			float y = blib::util::swapByteOrder(file->readFloat());
			object->position = glm::vec2(x, y);
		}
		{
			float x = blib::util::swapByteOrder(file->readFloat());
			float y = blib::util::swapByteOrder(file->readFloat());
			object->size = glm::vec2(x, y);
		}

		int nStrings = blib::util::swapByteOrder(file->readInt());
		for (int ii = 0; ii < nStrings; ii++)
		{
			std::string key = readString(file);
			std::string value = readString(file);
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			std::transform(value.begin(), value.end(), value.begin(), ::tolower);
			object->stringValues[key] = value;
		}
		int nFloats = blib::util::swapByteOrder(file->readInt());
		for (int ii = 0; ii < nFloats; ii++)
		{
			std::string key = readString(file);
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			float value = blib::util::swapByteOrder(file->readFloat());
			object->floatValues[key] = value;
		}

		objects.push_back(object);
	}

	for (int layer = 0; layer < 10 && !file->eof(); layer++)
	{
		std::list<DecorObject*> l;
		int nDecorObjects = blib::util::swapByteOrder(file->readInt());
		if (file->eof())
			break;
		for (int i = 0; i < nDecorObjects; i++)
		{
			DecorObject* object = new DecorObject();
			object->filename = readString(file);
			{
				int x = blib::util::swapByteOrder(file->readInt());
				int y = blib::util::swapByteOrder(file->readInt());
				object->position = glm::vec2(x, y);
			}
			object->fliph = file->get() != 0;
			object->flipv = file->get() != 0;
			object->rotation = blib::util::swapByteOrder(file->readInt());
			object->texInfo = textureMap->addTexture("assets/games/pewpew/textures/level/tiles/" + object->filename);
			if (!object->texInfo)
				object->texInfo = errorTexture;
			object->size = glm::vec2(object->texInfo->width, object->texInfo->height);
			l.push_back(object);
		}
		decorObjects.push_back(l);
	}
	delete file;


	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, 0.0f);
	body = world->CreateBody(&groundBodyDef);

	for (size_t p = 0; p < polygons.size(); p++)
	{
		std::vector<glm::vec2> &levelData = polygons[p];
		body->SetUserData(new WallCollision()); //TODO: clean this up
		b2Vec2* vs = new b2Vec2[levelData.size()];

		for (size_t i = 0; i < levelData.size(); i++)
			vs[i] = b2Vec2(levelData[i].x / 128, levelData[i].y / 128);
		b2ChainShape chain;
		chain.CreateLoop(vs, levelData.size());

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &chain;
		fixtureDef.restitution = 0.0f;//.5f;//0.95f;
		fixtureDef.filter.categoryBits = COL_LEVEL;
		fixtureDef.filter.maskBits = COL_ALL;
		body->CreateFixture(&fixtureDef);
	}


	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->init(this, world);
	}


}


Level::~Level()
{
	blib::linq::deleteall(objects);
	for (size_t i = 0; i < decorObjects.size(); i++)
		blib::linq::deleteall(decorObjects[i]);
	decorObjects.clear();

	delete textureMap;

	delete errorTexture;

	delete levelCache;
	delete levelOverlayCache;

	delete objectTextureMap;


}


void Level::draw(blib::SpriteBatch* spriteBatch)
{
	if (levelCache == NULL)
	{
		spriteBatch->startCache();
		for (int i = 0; i < 2; i++)
		{
			for (std::list<DecorObject*>::iterator it = decorObjects[i].begin(); it != decorObjects[i].end(); it++)
			{
				DecorObject* o = *it;

				glm::mat4 matrix;
				matrix = glm::translate(matrix, glm::vec3(o->position.x, o->position.y, 0));

				if (o->fliph)
				{
					matrix = glm::translate(matrix, glm::vec3(o->size.x / 2, 0, 0));
					matrix = glm::scale(matrix, glm::vec3(-1, 1, 1));
					matrix = glm::translate(matrix, glm::vec3(-o->size.x / 2, 0, 0));
				}
				if (o->flipv)
				{
					matrix = glm::translate(matrix, glm::vec3(0, o->size.y / 2, 0));
					matrix = glm::scale(matrix, glm::vec3(1, -1, 1));
					matrix = glm::translate(matrix, glm::vec3(0, -o->size.y / 2, 0));
				}

				matrix = glm::translate(matrix, glm::vec3(o->size * glm::vec2(0.5f, 0.5f), 0));
				matrix = glm::rotate(matrix, (float)o->rotation, glm::vec3(0, 0, 1));
				matrix = glm::translate(matrix, glm::vec3(o->size * glm::vec2(-0.5f, -0.5f), 0));
				spriteBatch->draw(o->texInfo, matrix);

			}
		}
		levelCache = spriteBatch->getCache();
	}
	else
		spriteBatch->drawCache(levelCache);

}

void Level::drawObjects(blib::SpriteBatch* spriteBatch)
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->type == "finish")
			Sleep(0);
		objects[i]->draw(spriteBatch);
	}
}

void Level::drawOverlay(blib::SpriteBatch* spriteBatch)
{
	if (levelOverlayCache == NULL)
	{
		spriteBatch->startCache();
		for (int i = 2; i < 10; i++)
		{
			for (std::list<DecorObject*>::iterator it = decorObjects[i].begin(); it != decorObjects[i].end(); it++)
			{
				DecorObject* o = *it;

				glm::mat4 matrix;
				matrix = glm::translate(matrix, glm::vec3(o->position.x, o->position.y, 0));

				if (o->fliph)
				{
					matrix = glm::translate(matrix, glm::vec3(o->size.x / 2, 0, 0));
					matrix = glm::scale(matrix, glm::vec3(-1, 1, 1));
					matrix = glm::translate(matrix, glm::vec3(-o->size.x / 2, 0, 0));
				}
				if (o->flipv)
				{
					matrix = glm::translate(matrix, glm::vec3(0, o->size.y / 2, 0));
					matrix = glm::scale(matrix, glm::vec3(1, -1, 1));
					matrix = glm::translate(matrix, glm::vec3(0, -o->size.y / 2, 0));
				}

				matrix = glm::translate(matrix, glm::vec3(o->size * glm::vec2(0.5f, 0.5f), 0));
				matrix = glm::rotate(matrix, (float)o->rotation, glm::vec3(0, 0, 1));
				matrix = glm::translate(matrix, glm::vec3(o->size * glm::vec2(-0.5f, -0.5f), 0));
				spriteBatch->draw(o->texInfo, matrix);

			}
		}
		levelOverlayCache = spriteBatch->getCache();
	}
	else
		spriteBatch->drawCache(levelOverlayCache);
}



void Level::update(float gameTime, float elapsedTime, const std::vector<PewPewPlayer*> &players)
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->type == "piston")
		{
			LevelObject<SpriteGameObject>* piston = (LevelObject<SpriteGameObject>*)objects[i];
			LevelObjectBase* targetObject = getObject(piston->stringValues[piston->stringValues["state"] + "anchor"]);
			if (!targetObject)
			{
				Log::out << "Could not find target " << piston->stringValues[piston->stringValues["state"] + "anchor"] << " for piston " << piston->name << Log::newline;
				continue;
			}

			glm::vec2 target = targetObject->getCenter();
			glm::vec2 pos = piston->getCenter();
			glm::vec2 diff(target - pos);
			if (glm::length(diff) > 0.01f)
			{
				diff = glm::normalize(diff);
				diff *= 0.5f;
				if (objects[i]->hasFloatValue("speed"))
					diff *= objects[i]->floatValues["speed"];
				piston->body->SetLinearVelocity(b2Vec2(diff.x, diff.y));
			}
			else
			{
				piston->body->SetTransform(b2Vec2(target.x, target.y), piston->body->GetAngle());
				piston->body->SetLinearVelocity(b2Vec2(0, 0));
			}
		}
		else if (objects[i]->type == "button")
		{
			ButtonLevelObject* button = (ButtonLevelObject*)objects[i];
			float extension = (button->body->GetWorldCenter() - button->anchor->GetWorldCenter()).Length();
			bool triggeredByObject = false;

			if (objects[i]->hasFloatValue("minweight"))
			{
				std::vector<b2Body*> touchingBodies;
				getTouchingBodies(button->body, touchingBodies);

				float weight = 0;

				for (size_t ii = 0; ii < touchingBodies.size(); ii++)
				{
					CollisionProperties* props = (CollisionProperties*)touchingBodies[ii]->GetUserData();
					if (props)
					{
						if (props->type == CollisionProperties::GRAVITYOBJECT)
						{
							GravityObjectCollision* gravProps = (GravityObjectCollision*)props;
							if (gravProps->object->hasFloatValue("weight"))
								weight += gravProps->object->floatValues["weight"];
						}
					}
				}
				if (weight >= objects[i]->floatValues["minweight"])
					triggeredByObject = true;
			}


			if (extension > 0.1f || triggeredByObject)
			{
				if (button->lastTriggerTime < gameTime)
				{
					if (button->texture2 && button->texture1)
						button->sprite = (button->sprite == button->texture1) ? button->texture2 : button->texture1;

					triggerActions(button->stringValues["triggeredobject"], button->stringValues["triggeraction"]);

				}
				button->lastTriggerTime = gameTime + 0.1f;
			}
		}
		else if (objects[i]->type == "timer")
		{
			if (!objects[i]->hasFloatValue("lasttime"))
				objects[i]->floatValues["lasttime"] = gameTime + objects[i]->floatValues["delay"];
			if (gameTime > objects[i]->floatValues["lasttime"] && objects[i]->floatValues["lasttime"] > 0)
			{
				objects[i]->floatValues["lasttime"] = gameTime + objects[i]->floatValues["delay"];
				triggerActions(objects[i]->stringValues["triggeredobject"], objects[i]->stringValues["triggeraction"]);
			}
		}
		else if (objects[i]->type == "wind")
		{
			/*glm::vec2 playerPos = playerBody->GetWorldCenter();
			playerPos *= 128.0f;
			if (objects[i]->contains(playerPos))
			{
				playerBody->ApplyForceToCenter(b2Vec2(5.1f, 0));
			}*/
		} 
		else if (objects[i]->type == "trigger")
		{ 
			for (auto p : players)
			{
				glm::vec2 playerPos = p->body->GetWorldCenter();
				playerPos *= 128.0f;
				if (objects[i]->contains(playerPos))
				{
					triggerActions(objects[i]->stringValues["triggeredobject"], objects[i]->stringValues["triggeraction"]);
				}
			}
		}
	}
}





void Level::triggerActions(std::string objectnames, std::string a)
{
	std::vector<std::string> names = blib::util::split(objectnames, ",");
	std::vector<std::string> actions = blib::util::split(a, ",");
	if (actions.size() == 1)
		actions.resize(names.size(), actions[0]);

	for (size_t i = 0; i < names.size(); i++)
	{
		std::vector<LevelObjectBase*> objects = getObjects(names[i]);
		for (size_t ii = 0; ii < objects.size(); ii++)
			objects[i]->doAction(actions[i]);
	}

}





void Level::cleanWorld(b2World* world)
{
	world->DestroyBody(body);
	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->cleanWorld(world);
	}
}





LevelObjectBase* Level::getObject(std::string name)
{
	std::vector<LevelObjectBase*> objects = getObjects(name);
	if (objects.size() == 0)
		return NULL;

	return objects[0];
}
std::vector<LevelObjectBase*> Level::getObjects(std::string name)
{
	std::vector<LevelObjectBase*> result;

	std::map<std::string, std::vector<LevelObjectBase*> >::iterator it = objectCache.find(name);
	if (it != objectCache.end())
		return it->second;

	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i]->name == name)
			result.push_back(objects[i]);
	objectCache[name] = result;
	return result;
}

void Level::getTouchingBodies(b2Body* body, std::vector<b2Body*> &touchingBodies)
{
	CollisionProperties* properties = (CollisionProperties*)body->GetUserData();
	if (!properties)
		return;

	if (properties->type == CollisionProperties::GRAVITYOBJECT || properties->type == CollisionProperties::BUTTON)
	{
		if (!blib::linq::contains(touchingBodies, [body](b2Body* b) { return b == body; }))
			touchingBodies.push_back(body);
		else
			return;

		b2ContactEdge* contact = body->GetContactList();
		while (contact)
		{
			if (contact->contact->GetFixtureA()->GetBody() != body)
				getTouchingBodies(contact->contact->GetFixtureA()->GetBody(), touchingBodies);
			if (contact->contact->GetFixtureB()->GetBody() != body)
				getTouchingBodies(contact->contact->GetFixtureB()->GetBody(), touchingBodies);
			contact = contact->next;
		}
	}

}

std::vector<Enemy*> Level::getEnemies() const
{
	std::vector<Enemy*> ret;
	for (size_t i = 0; i < objects.size(); i++)
		if (objects[i]->type == "enemy")
			ret.push_back(static_cast<Enemy*>(objects[i]));

	return ret;
}


