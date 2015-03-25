#include "TowerTopple.h"

#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>
#include <blib/Renderer.h>
#include <blib/math/Ray.h>
#include <blib/linq.h>
#include <blib/StaticModel.h>
#include <blib/util/Log.h>
using blib::util::Log;

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define FLOOR 1
#define BALL 2
#define BOX 3

std::string TowerTopple::getName()
{
	return "TowerTopple";
}

std::string TowerTopple::getInstructions()
{
	return "Topple the tower";
}

void TowerTopple::loadResources()
{
	renderState = spriteBatch->renderState;
	renderState.depthTest = true;
	renderState.activeShader = resourceManager->getResource<blib::Shader>("TowerTopple");
	renderState.activeShader->bindAttributeLocation("a_position", 0);
	renderState.activeShader->bindAttributeLocation("a_texcoord", 1);
	renderState.activeShader->bindAttributeLocation("a_normal", 2);
	renderState.activeShader->setUniformName(Uniforms::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	renderState.activeShader->setUniformName(Uniforms::ModelMatrix, "modelMatrix", blib::Shader::Mat4);
	renderState.activeShader->setUniformName(Uniforms::CameraMatrix, "cameraMatrix", blib::Shader::Mat4);
	renderState.activeShader->setUniformName(Uniforms::s_texture, "s_texture", blib::Shader::Int);
	renderState.activeShader->setUniformName(Uniforms::color, "color", blib::Shader::Vec4);
	renderState.activeShader->finishUniformSetup();
	renderState.activeShader->setUniform(Uniforms::s_texture, 0);
	renderState.activeVbo = NULL;
	renderState.activeVio = NULL;

	testTexture = resourceManager->getResource<blib::Texture>("assets/games/TowerTopple/test.png");
	testTexture->setTextureRepeat(true);
	outlineTexture = resourceManager->getResource<blib::Texture>("assets/games/TowerTopple/outline.png");
	outlineTexture->nearest = true;

	ballsTexture = resourceManager->getResource<blib::Texture>("assets/games/TowerTopple/balls.png");
	cursorSprite = resourceManager->getResource<blib::Texture>("assets/cursor.png");

	font = resourceManager->getResource<blib::Font>("lindsey");


	ballModel = new blib::StaticModel("assets/games/TowerTopple/ball.3ds.json", resourceManager, renderer);
}

void TowerTopple::start(Difficulty difficulty)
{
	//TODO: delete old stuff
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	world->setGravity(btVector3(0, -9.8f, 0));
	//	gContactAddedCallback = CustomMaterialCombinerCallback;
	debugDraw = new DebugDraw(resourceManager);
	debugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	world->setDebugDrawer(debugDraw);

	{
		btBoxShape* groundShape = new btBoxShape(btVector3(6.5f, 1, 6.5f));
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -1, 0));
		btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo cInfo(0, groundMotionState, groundShape, btVector3(1, 1, 1));
		btRigidBody* floorBody = new btRigidBody(cInfo);
		floorBody->setFriction(1.0);
		world->addRigidBody(floorBody);
		groundBody = floorBody;
		groundBody->setUserIndex(FLOOR);
	}
	boxes.clear();


	for (auto p : players)
	{
		p->cursor = settings->center + 250.0f * blib::util::fromAngle(p->index * blib::math::pif * 2 / players.size());
	}


	int layer = 0;
	auto addTinyLayer = [this, &layer]()
	{
		for (int x = -2; x <= 2; x++)
			for (int y = -2; y <= 2; y++)
				addBox(btVector3(x, layer + 0.5f, y), btVector3(0.5f, 0.5f, 0.5f));
		layer++;
	};
	
	auto addLayer1 = [this, &layer](){
		addBox(btVector3(-5.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(-3.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(-1.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(1.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(3.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(5.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));

		addBox(btVector3(-5.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(-3.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(-1.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(1.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(3.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(5.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));

		addBox(btVector3(-5.5f, layer + 0.5f, -4.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(-5.5f, layer + 0.5f, -2.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(-5.5f, layer + 0.5f, 0.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(-5.5f, layer + 0.5f, 2.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(-5.5f, layer + 0.5f, 4.0f), btVector3(0.5f, 0.5f, 1.0f));

		addBox(btVector3(5.5f, layer + 0.5f, -4.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(5.5f, layer + 0.5f, -2.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(5.5f, layer + 0.5f, 0.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(5.5f, layer + 0.5f, 2.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(5.5f, layer + 0.5f, 4.0f), btVector3(0.5f, 0.5f, 1.0f));
		layer++;
	};

	auto addLayer2 = [this, &layer](){
		addBox(btVector3(-4.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(-2.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(0.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(2.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(4.0f, layer + 0.5f, 5.5f), btVector3(1.0f, 0.5f, 0.5f));

		addBox(btVector3(-4.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(-2.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(0.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(2.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));
		addBox(btVector3(4.0f, layer + 0.5f, -5.5f), btVector3(1.0f, 0.5f, 0.5f));

		addBox(btVector3(-5.5f, layer + 0.5f, -5.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(-5.5f, layer + 0.5f, -3.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(-5.5f, layer + 0.5f, -1.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(-5.5f, layer + 0.5f, 1.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(-5.5f, layer + 0.5f, 3.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(-5.5f, layer + 0.5f, 5.0f), btVector3(0.5f, 0.5f, 1.0f));

		addBox(btVector3(5.5f, layer + 0.5f, -5.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(5.5f, layer + 0.5f, -3.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(5.5f, layer + 0.5f, -1.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(5.5f, layer + 0.5f, 1.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(5.5f, layer + 0.5f, 3.0f), btVector3(0.5f, 0.5f, 1.0f));
		addBox(btVector3(5.5f, layer + 0.5f, 5.0f), btVector3(0.5f, 0.5f, 1.0f));
		layer++;
	};

	for (int i = 0; i < 3; i++)
	{
		addBox(btVector3(-2.5f, layer + 2.5f, -2.5f), btVector3(2.5f, 2.5f, 2.5f));
		addBox(btVector3(2.5f, layer + 2.5f, -2.5f), btVector3(2.5f, 2.5f, 2.5f));
		addBox(btVector3(2.5f, layer + 2.5f, 2.5f), btVector3(2.5f, 2.5f, 2.5f));
		addBox(btVector3(-2.5f, layer + 2.5f, 2.5f), btVector3(2.5f, 2.5f, 2.5f));


		addLayer1();
		addLayer2();
		addLayer1();
		addLayer2();
		addLayer1();
		addBox(btVector3(0.0f, layer + 0.5f, 0.0f), btVector3(6.0f, 0.5f, 6.0f)); layer++;
	}


	world->stepSimulation(10, 1000);
}

void TowerTopple::addBox(const btVector3& pos, const btVector3& extends)
{
	btBoxShape* groundShape = new btBoxShape(extends);
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(pos);
	//groundTransform.setRotation(btQuaternion(blib::math::randomFloat(0, 360), 0, 0));

	float mass = (extends.x() * 2) * (extends.y() * 2) * (extends.z() * 2);

	btVector3 fallInertia;
	groundShape->calculateLocalInertia(mass, fallInertia);

	btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, groundMotionState, groundShape, fallInertia);
	btRigidBody* floorBody = new btRigidBody(cInfo);
	floorBody->setUserIndex(BOX);
	world->addRigidBody(floorBody);
	floorBody->setFriction(0.99);

	boxes.push_back(new Box());
	boxes[boxes.size() - 1]->body = floorBody;
	boxes[boxes.size() - 1]->extends = extends;
}

void TowerTopple::update( float elapsedTime )
{
	world->stepSimulation((float)elapsedTime, 4);


//	camDist += players[0]->joystick.leftStick.y * elapsedTime * 5;
//	camAngle += players[0]->joystick.leftStick.x * elapsedTime * 90;


	camRot += elapsedTime*5;





	for (auto p : players)
	{
		p->ballTImer = glm::min(p->ballTImer + (float)elapsedTime, 5.0f);
		p->cursor += elapsedTime * p->joystick.leftStick * 750.0f;

		//if (!p->bla)
		{

			glm::vec2 cursor = p->cursor;
			cursor -= settings->center;
			cursor *= 1.25f;
			cursor += settings->center;

			glm::mat4 projectionMatrix = glm::perspective(75.0f, (float)settings->resX / settings->resY, 0.1f, 500.0f);
			glm::mat4 cameraMatrix;
			cameraMatrix = glm::rotate(cameraMatrix, -10.0f, glm::vec3(1, 0, 0));
			cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0, 0, -camDist));
			cameraMatrix = glm::rotate(cameraMatrix, camAngle, glm::vec3(1, 0, 0));
			cameraMatrix = glm::rotate(cameraMatrix, camRot, glm::vec3(0, 1, 0));
			renderer->unproject(cursor, NULL, &p->shootRay, cameraMatrix, projectionMatrix);
			p->bla = true;
		}
		if (p->joystick.a == 1 && p->prevJoystick.a == 0 && p->ballTImer > 1)
		{
			p->ballTImer--;
			btSphereShape* groundShape = new btSphereShape(0.5f);
			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(p->shootRay.origin.x, p->shootRay.origin.y, p->shootRay.origin.z));

			float mass = 25;
			btVector3 fallInertia;
			groundShape->calculateLocalInertia(mass, fallInertia);

			btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo cInfo(mass, groundMotionState, groundShape, fallInertia);
			btRigidBody* ballBody = new btRigidBody(cInfo);
			ballBody->setUserIndex(BALL);
			world->addRigidBody(ballBody);
			ballBody->setFriction(1.0);
			ballBody->applyCentralImpulse(btVector3(1250 * p->shootRay.dir.x, 1250 * p->shootRay.dir.y, 1250 * p->shootRay.dir.z));
			p->balls.push_back(ballBody);

		}
	}


	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;



				for (auto p : players)
				{
					if (obA->getUserIndex() == BALL && blib::linq::containsValue(p->balls, obA))
					{
						if (obB->getUserIndex() == BOX)
						{
							Box* b = blib::linq::firstOrDefault<Box*>(boxes, [this, &obB](Box* b) { return b->body == obB; }, nullptr);
							b->player = p;
						}
					}
					if (obB->getUserIndex() == BALL && blib::linq::containsValue(p->balls, obB))
					{
						if (obA->getUserIndex() == BOX)
						{
							Box* b = blib::linq::firstOrDefault<Box*>(boxes, [this, &obA](Box* b) { return b->body == obA; }, nullptr);
							b->player = p;
						}
					}
				}


			}
		}
	}


	for (auto b : boxes)
	{
		if (b->body->isInWorld())
		{
			if (b->body->getWorldTransform().getOrigin().y() < 0)
			{
				world->removeRigidBody(b->body);
				if (b->player)
					b->player->score += (b->extends.x() * 2) * (b->extends.y() * 2) * (b->extends.z() * 2);
			}
		}
	}

}

void TowerTopple::draw()
{
	glm::mat4 projectionMatrix = glm::perspective(90.0f, (float)settings->resX / settings->resY, 0.1f, 500.0f);
//	glm::mat4 cameraMatrix = glm::lookAt(glm::vec3(0, -5, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	renderState.activeShader->setUniform(Uniforms::color, glm::vec4(1, 1, 1, 1));


	float s = 0.5f;
	std::vector<blib::VertexP3T2N3> boxVerts;
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s, -s, -s), glm::vec2(0, 0), glm::vec3(0, 1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(s,  -s, -s), glm::vec2(1, 0), glm::vec3(0, 1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s, -s, s), glm::vec2(0, 1), glm::vec3(0, 1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(s,  -s, s), glm::vec2(1, 1), glm::vec3(0, 1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s, -s, s), glm::vec2(0, 1), glm::vec3(0, 1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(s,  -s, -s), glm::vec2(1, 0), glm::vec3(0, 1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s, -s), glm::vec2(0, 0), glm::vec3(0,-1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(s,   s, -s), glm::vec2(1, 0), glm::vec3(0,-1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s, s), glm::vec2(0, 1), glm::vec3(0,-1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(s,   s, s), glm::vec2(1, 1), glm::vec3(0,-1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s, s), glm::vec2(0, 0), glm::vec3(0,-1, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(s,   s, -s), glm::vec2(1, 1), glm::vec3(0,-1, 0)));

	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s, -s, -s), glm::vec2(0, 0), glm::vec3(1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s, -s), glm::vec2(1, 0), glm::vec3(1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s, -s, s), glm::vec2(0, 1), glm::vec3(1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s, s), glm::vec2(1, 1), glm::vec3(1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s, -s, s), glm::vec2(0, 1), glm::vec3(1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s, -s), glm::vec2(1, 0), glm::vec3(1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s, -s, -s), glm::vec2(0, 0), glm::vec3(-1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s,  s, -s), glm::vec2(1, 0), glm::vec3(-1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s, -s, s), glm::vec2(0, 1), glm::vec3(-1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s,  s, s), glm::vec2(1, 1), glm::vec3(-1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s, -s, s), glm::vec2(0, 1), glm::vec3(-1, 0, 0)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s,  s, -s), glm::vec2(1, 0), glm::vec3(-1, 0, 0)));

	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s, -s, -s), glm::vec2(0, 0), glm::vec3(0, 0, 1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s, -s), glm::vec2(0, 1), glm::vec3(0, 0, 1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s, -s, -s), glm::vec2(1, 0), glm::vec3(0, 0, 1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s,  s, -s), glm::vec2(1, 1), glm::vec3(0, 0, 1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s, -s, -s), glm::vec2(1, 0), glm::vec3(0, 0, 1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s, -s), glm::vec2(0, 1), glm::vec3(0, 0, 1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s, -s,  s), glm::vec2(0, 0), glm::vec3(0, 0,-1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s,  s), glm::vec2(0, 1), glm::vec3(0, 0,-1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s, -s,  s), glm::vec2(1, 0), glm::vec3(0, 0,-1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s,  s,  s), glm::vec2(1, 1), glm::vec3(0, 0,-1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3( s, -s,  s), glm::vec2(1, 0), glm::vec3(0, 0,-1)));
	boxVerts.push_back(blib::VertexP3T2N3(glm::vec3(-s,  s,  s), glm::vec2(0, 1), glm::vec3(0, 0,-1)));



	glm::mat4 cameraMatrix;
	cameraMatrix = glm::rotate(cameraMatrix, -10.0f, glm::vec3(1, 0, 0));
	cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0, 0, -camDist));
	cameraMatrix = glm::rotate(cameraMatrix, camAngle, glm::vec3(1, 0, 0));
	cameraMatrix = glm::rotate(cameraMatrix, camRot, glm::vec3(0, 1, 0));


	float h = 0;
	std::vector<blib::VertexP3T2N3> verts;
	verts.push_back(blib::VertexP3T2N3(glm::vec3(-7, h, -7), glm::vec2(0, 0), glm::vec3(0, 1, 0)));
	verts.push_back(blib::VertexP3T2N3(glm::vec3(7, h, 7), glm::vec2(10, 10), glm::vec3(0, 1, 0)));
	verts.push_back(blib::VertexP3T2N3(glm::vec3(7, h, -7), glm::vec2(10, 0), glm::vec3(0, 1, 0)));

	verts.push_back(blib::VertexP3T2N3(glm::vec3(-7, h, -7), glm::vec2(0, 0), glm::vec3(0, 1, 0)));
	verts.push_back(blib::VertexP3T2N3(glm::vec3(7, h, 7), glm::vec2(10, 10), glm::vec3(0, 1, 0)));
	verts.push_back(blib::VertexP3T2N3(glm::vec3(-7, h, 7), glm::vec2(0, 10), glm::vec3(0, 1, 0)));

	renderState.activeShader->setUniform(Uniforms::CameraMatrix, cameraMatrix);
	renderState.activeShader->setUniform(Uniforms::ProjectionMatrix, projectionMatrix);
	renderState.activeShader->setUniform(Uniforms::ModelMatrix, glm::mat4());
	renderState.activeFbo = spriteBatch->renderState.activeFbo;
	renderState.activeTexture[0] = testTexture;

	renderer->drawTriangles(verts, renderState);


	renderState.cullFaces = blib::RenderState::CullFaces::NONE;
	renderState.depthTest = true;

	renderState.activeTexture[0] = outlineTexture;
	for (auto b : boxes)
	{
		if (!b->body->isInWorld())
			continue;
		btScalar m[16];
		b->body->getWorldTransform().getOpenGLMatrix(m);
		renderState.activeShader->setUniform(Uniforms::ModelMatrix, glm::scale(glm::make_mat4(m), glm::vec3(b->extends.x() * 2, b->extends.y() * 2, b->extends.z() * 2)));
		renderState.activeShader->setUniform(Uniforms::color, b->player == NULL ? glm::vec4(1, 1, 1, 1) : b->player->participant->color);

		renderer->drawTriangles(boxVerts, renderState);


	}
	renderState.activeShader->setUniform(Uniforms::color, glm::vec4(1, 1, 1, 1));

	for (auto p : players)
	{
		renderState.activeShader->setUniform(Uniforms::color, p->participant->color);
		for (auto ball : p->balls)
		{
			btScalar m[16];
			ball->getWorldTransform().getOpenGLMatrix(m);
			renderState.activeShader->setUniform(Uniforms::ModelMatrix, glm::scale(glm::make_mat4(m), glm::vec3(1,1,1)));

			ballModel->draw(renderState, renderer, -1);
		}
	}


	
	blib::RenderState r = debugDraw->renderstate;
	r.activeShader->setUniform(DebugDraw::Uniforms::ProjectionMatrix, projectionMatrix);
	r.activeShader->setUniform(DebugDraw::Uniforms::CameraMatrix, cameraMatrix);
	std::vector<blib::VertexP3C4> vertices;

	for (auto p : players)
	{
		vertices.push_back(blib::VertexP3C4(p->shootRay.origin, glm::vec4(1, 0, 0, 1)));
		vertices.push_back(blib::VertexP3C4(p->shootRay.origin + 10.0f * p->shootRay.dir, glm::vec4(1, 0, 0, 1)));
	}

	renderer->drawLines(vertices, r);


	spriteBatch->begin();
	for (auto p : players)
	{
		spriteBatch->draw(cursorSprite, blib::math::easyMatrix(p->cursor), p->participant->color);
		spriteBatch->draw(font, std::to_string(p->score), blib::math::easyMatrix(glm::vec2(20, 50 * p->index + 30)), p->participant->color);
		spriteBatch->draw(ballsTexture, blib::math::easyMatrix(glm::vec2(100, 50 * p->index + 50)));
		spriteBatch->draw(ballsTexture, blib::math::easyMatrix(glm::vec2(100, 50 * p->index + 50)), glm::vec2(0,0), blib::math::Rectangle(glm::vec2(0,0), p->ballTImer / 5.0f,1.0f), p->participant->color);

	}
	spriteBatch->end();


	/*world->debugDrawWorld();
	debugDraw->renderstate.activeFbo = spriteBatch->renderState.activeFbo;
	debugDraw->flush(renderer, projectionMatrix, cameraMatrix);*/


}

blib::Texture* TowerTopple::getTitleImage()
{
	return NULL;
}

bool TowerTopple::hasWinner()
{
	return !blib::linq::any(boxes, [](Box* b) { return b->body->isInWorld(); });


}




DebugDraw::DebugDraw(blib::ResourceManager* resourceManager)
{
	renderstate.activeShader = resourceManager->getResource<blib::Shader>("bulletdebug");
	renderstate.activeShader->setUniformName(Uniforms::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
	renderstate.activeShader->setUniformName(Uniforms::CameraMatrix, "cameraMatrix", blib::Shader::Mat4);
	renderstate.activeShader->finishUniformSetup();
	renderstate.depthTest = false;
}


void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	if (vertices.size() > 1000000)
		return;
	glm::vec4 c(color.m_floats[0], color.m_floats[1], color.m_floats[2], 1.0f);
	vertices.push_back(blib::VertexP3C4(glm::vec3(from.m_floats[0], from.m_floats[1], from.m_floats[2]), c));
	vertices.push_back(blib::VertexP3C4(glm::vec3(to.m_floats[0], to.m_floats[1], to.m_floats[2]), c));
}

void DebugDraw::flush(blib::Renderer* renderer, const glm::mat4 &projectionMatrix, const glm::mat4 &cameraMatrix)
{
	if (!vertices.empty())
	{
		renderstate.activeShader->setUniform(Uniforms::ProjectionMatrix, projectionMatrix);
		renderstate.activeShader->setUniform(Uniforms::CameraMatrix, cameraMatrix);
		renderer->drawLines(vertices, renderstate);
		vertices.clear();
	}
}
