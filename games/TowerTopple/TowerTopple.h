#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"
#include "TowerTopplePlayer.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include <blib/RenderState.h>
#include <blib/gl/Vertex.h>

namespace blib { class Texture; class Font;  }

class DebugDraw : public btIDebugDraw
{
	std::vector<blib::VertexP3C4> vertices;
	int debugMode;
public:
	blib::RenderState renderstate;
	enum class Uniforms
	{
		ProjectionMatrix,
		CameraMatrix,
	};

	DebugDraw(blib::ResourceManager* resourceManager);
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)	{	}

	void flush(blib::Renderer* renderer, const glm::mat4 &projectionMatrix, const glm::mat4 &cameraMatrix);

	virtual void reportErrorWarning(const char* warningString)		{ printf("%s\n", warningString); }
	virtual void draw3dText(const btVector3& location, const char* textString)		{	}
	virtual void setDebugMode(int debugMode)
	{
		this->debugMode = debugMode;
	}
	virtual int getDebugMode() const
	{
		return debugMode;
	}
};


class Box
{
public:
	btRigidBody* body;
	btVector3 extends;
};

class TowerTopple : public ScoreGame<TowerTopplePlayer>
{
public:
	btBroadphaseInterface*                  broadphase;
	btDefaultCollisionConfiguration*        collisionConfiguration;
	btCollisionDispatcher*                  dispatcher;
	btSequentialImpulseConstraintSolver*    solver;
	btDiscreteDynamicsWorld*                world;
	DebugDraw* debugDraw;

	blib::RenderState renderState;
	enum class Uniforms
	{
		ProjectionMatrix,
		CameraMatrix,
		ModelMatrix,
		s_texture
	};
	blib::Texture* testTexture;
	blib::Texture* outlineTexture;
	std::vector<Box*> boxes;

	float camAngle = 50;
	float camDist = 10;

	void addBox(const btVector3& pos, const btVector3& extends);

	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};







extern "C" {
	__declspec( dllexport ) GameBase* getGame();
}