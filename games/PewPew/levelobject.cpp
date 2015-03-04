#include "levelobject.h"
#include "level.h"
#include "CollisionProps.h"
#include "collisionlayers.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <Box2D/Box2D.h>

#include <blib/Util.h>
#include <blib/util/StreamIn.h>
#include <blib/util/FileSystem.h>
#include <blib/util/Log.h>

using blib::util::Log;


template<>
void LevelObject<SpriteGameObject>::myInit(Level* level)
{
	sprite = level->objectTextureMap->addTexture("assets/games/pewpew/textures/level/objects/" + stringValues["texture"]);
	if(hasStringValue("texture2"))
	{
		texture1 = sprite;
		texture2 = level->objectTextureMap->addTexture("assets/games/pewpew/textures/level/objects/" + stringValues["texture2"]);
	}
}



void LevelObjectBase::doAction(std::string action)
{
	if(type == "piston")
	{
		if(action == "pistontoggle")
			stringValues["state"] = stringValues["state"] == "open" ? "closed" : "open";
		if(action == "pistonopen")
			stringValues["state"] = "open";
		if(action == "pistonclose")
			stringValues["state"] = "close";
	}
}




template <>
glm::mat4 LevelObject<SpriteGameObject>::getMatrix() const { return glm::scale(GameObject<SpriteGameObject, PhysicsGameObject>::getMatrix(this), glm::vec3(size.x / sprite->width, size.y / sprite->height, 1.0f)); }

template <>
glm::mat4 LevelObject<InvisibleGameObject>::getMatrix() const { return GameObject<InvisibleGameObject, PhysicsGameObject>::getMatrix(this); };

template <>
glm::mat4 LevelObject<AnimationGameObject>::getMatrix() const { return GameObject<AnimationGameObject, PhysicsGameObject>::getMatrix(this); };


template <class animationtype>
void LevelObject<animationtype>::init( Level* level, b2World* world )
{
	myInit(level);

	if(type == "gravityobject" || type == "piston" || type == "engine" || type == "trampoline" || type == "spikes" || type == "anchor" || type == "button")
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		if(type == "piston" || type == "engine" || type == "trampoline" || type == "spikes")
			bodyDef.type = b2_kinematicBody;

		if(hasStringValue("movable"))
			if(stringValues["movable"] == "false")
				bodyDef.type = b2_kinematicBody;

		if(type == "anchor")
			bodyDef.type = b2_staticBody;


		bodyDef.position.Set((position.x+size.x/2.0f)/128, (position.y+size.y/2.0f)/128.0f);
		bodyDef.angle = glm::radians(0.0f);
		if(hasFloatValue("angle"))
			bodyDef.angle = glm::radians(floatValues["angle"]);
		bodyDef.angularVelocity = 0;
		bodyDef.allowSleep = false;

		/*b2Body* */body = world->CreateBody(&bodyDef);
		body->SetUserData(new GravityObjectCollision(this));

		if(type == "spikes" || type == "anchor")
			stringValues["shape"] = "box";
		if(type == "piston" || type == "button")
			stringValues["shape"] = "file";



		if(hasFloatValue("gravity"))
			body->SetGravityScale(floatValues["gravity"]);


		std::list<b2Shape*> shapes = buildShape(stringValues["shape"]);	
		for(std::list<b2Shape*>::iterator it = shapes.begin(); it != shapes.end(); it++)
		{
			b2FixtureDef fixtureDef;
			fixtureDef.shape = *it;

			fixtureDef.density = 10.0f;//non-0 = static
			fixtureDef.friction = 1.0f;
			fixtureDef.restitution = 0.2f;
			fixtureDef.filter.categoryBits = COL_OBJECTS;
			fixtureDef.filter.maskBits = COL_ALL - COL_BUTTERFLY - COL_SCARF;

			if(type == "trampoline")
				fixtureDef.restitution = 0.99f;
			else if(type == "anchor")
			{
				fixtureDef.filter.categoryBits = 0;
				fixtureDef.filter.maskBits = 0;
			}
			else if(type == "button")
			{
				if(stringValues["texture"] == "pressureplate.png")
				{
					fixtureDef.density = 1000.0f;
					fixtureDef.restitution = 0;
				}
				else
					fixtureDef.density = 0.1f;
			}

			body->CreateFixture(&fixtureDef);
		}
	}
}




void ButtonLevelObject::init(Level* level, b2World* world)
{
	lastTriggerTime = 0;

	LevelObject<SpriteGameObject>::init(level, world);
	body->SetGravityScale(0);
	body->SetUserData(new ButtonCollision(this));

	b2BodyDef anchorDef;
	anchorDef.type = b2_staticBody;
	anchorDef.position.Set(body->GetWorldCenter().x, body->GetWorldCenter().y);
	anchor = world->CreateBody(&anchorDef);
	b2PolygonShape anchorShape;
	anchorShape.SetAsBox(size.x/128.0f/8.0f, size.y/128.0f/8.0f);

	b2FixtureDef anchorFixtureDef;
	anchorFixtureDef.filter.maskBits = 0;
	anchorFixtureDef.shape = &anchorShape;
	anchorFixtureDef.density = 0;//non-0 = static
	anchor->CreateFixture(&anchorFixtureDef);

	b2Vec2 center = body->GetWorldCenter();
	b2DistanceJointDef jointDef;
	jointDef.Initialize(anchor, body, anchor->GetWorldCenter(), body->GetWorldCenter());
	jointDef.collideConnected = false;
	jointDef.dampingRatio = 2.0f;
	jointDef.frequencyHz = 1.5f;
	joint1 = (b2DistanceJoint*)world->CreateJoint(&jointDef);

	b2Vec2 axis(glm::cos(glm::radians(floatValues["angle"]+90)), glm::sin(glm::radians(floatValues["angle"]+90)));
	b2PrismaticJointDef joint2Def;
	joint2Def.Initialize(anchor, body, anchor->GetWorldCenter(), axis);
	joint2 = world->CreateJoint(&joint2Def);
}

void ButtonLevelObject::cleanWorld(b2World* world)
{
	world->DestroyJoint(joint1);
	world->DestroyJoint(joint2);
	LevelObject<SpriteGameObject>::cleanWorld(world);
	world->DestroyBody(anchor);
}












std::list<b2Shape*> LevelObjectBase::buildShape( const std::string &shapeName, const std::string &filename)
{
	std::list<b2Shape*> ret;

	if(stringValues["shape"] == "circle")
	{
		b2CircleShape* shape = new b2CircleShape();
		shape->m_radius = size.x/128/2; //TODO: ellipse?
		ret.push_back(shape);
	}
	else if(stringValues["shape"] == "box")
	{
		b2PolygonShape* shape = new b2PolygonShape();
		shape->SetAsBox(size.x/128.0f/2.0f, size.y/128.0f/2.0f);
		ret.push_back(shape);
	}
	else if(stringValues["shape"] == "file")
	{
		blib::util::StreamIn* colFile = NULL;
		if(filename == "")
			colFile = blib::util::FileSystem::openRead("assets/games/pewpew/textures/level/objects/" + stringValues["texture"] + ".col");
		else
			colFile = blib::util::FileSystem::openRead("assets/games/pewpew/textures/level/objects/" + filename + ".col");
		int nVertices = blib::util::swapByteOrder(colFile->readInt());
		std::vector<b2Vec2> colVertices;
		for(int ii = 0; ii < nVertices; ii++)
		{
			float x = blib::util::swapByteOrder(colFile->readFloat());
			float y = blib::util::swapByteOrder(colFile->readFloat());
			colVertices.push_back(b2Vec2(x/128.0f,y/128.0f));
		}
		int nPolygons = blib::util::swapByteOrder(colFile->readInt());

		for(int ii = 0; ii < nPolygons; ii++)
		{
			int v1 = blib::util::swapByteOrder(colFile->readInt());
			int v2 = blib::util::swapByteOrder(colFile->readInt());
			int v3 = blib::util::swapByteOrder(colFile->readInt());

			b2PolygonShape* shape = new b2PolygonShape();
			b2Vec2 colVerts[3];
			colVerts[0] = colVertices[v1];
			colVerts[1] = colVertices[v2];
			colVerts[2] = colVertices[v3];

			//ccw determination
			float total = 0;
			total += colVertices[v2].y*colVertices[v1].x  - colVertices[v2].x*colVertices[v1].y;
			total += colVertices[v3].y*colVertices[v2].x  - colVertices[v3].x*colVertices[v2].y;
			total += colVertices[v1].y*colVertices[v3].x  - colVertices[v1].x*colVertices[v3].y;
			bool ccw = total < 0;
			if(ccw)
				std::swap(colVerts[0], colVerts[2]);

			shape->Set(colVerts, 3);
			ret.push_back(shape);
		}
		delete colFile;
	}
	else
	{
		Log::out<<"Unknown collision type! "<<stringValues["shape"]<<Log::newline;
	}
	return ret;
}





bool LevelObjectBase::hasStringValue( std::string name )
{
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	return stringValues.find(name) != stringValues.end();
}

bool LevelObjectBase::hasFloatValue( std::string name )
{
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	return floatValues.find(name) != floatValues.end();
}

bool LevelObjectBase::contains( glm::vec2 point )
{
	return point.x > position.x && point.x < position.x + size.x &&
		point.y > position.y && point.y < position.y + size.y;
}

glm::vec2 LevelObjectBase::getCenter()
{
	return position + size*glm::vec2(0.5f, -0.5f);
}




template class LevelObject<SpriteGameObject>;
template class LevelObject<InvisibleGameObject>;
template class LevelObject<AnimationGameObject>;
