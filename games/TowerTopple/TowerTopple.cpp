#include "TowerTopple.h"

#include <blib/SpriteBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>
#include <blib/Renderer.h>
#include <blib/math/Ray.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameBase* getGame()
{
	return new TowerTopple();
}

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
	renderState.activeShader->finishUniformSetup();
	renderState.activeShader->setUniform(Uniforms::s_texture, 0);
	renderState.activeVbo = NULL;
	renderState.activeVio = NULL;

	testTexture = resourceManager->getResource<blib::Texture>("assets/games/TowerTopple/test.png");
	testTexture->setTextureRepeat(true);
	outlineTexture = resourceManager->getResource<blib::Texture>("assets/games/TowerTopple/outline.png");
	outlineTexture->nearest = true;
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
		btBoxShape* groundShape = new btBoxShape(btVector3(10, 1, 10));
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -1, 0));
		btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo cInfo(0, groundMotionState, groundShape, btVector3(0, 0, 0));
		btRigidBody* floorBody = new btRigidBody(cInfo);
		floorBody->setFriction(1.0);
		world->addRigidBody(floorBody);
	}

	boxes.clear();

	int layer = 0;

	auto addTinyLayer = [this, &layer]()
	{
		for (int x = -2; x <= 2; x++)
			for (int y = -2; y <= 2; y++)
				addBox(btVector3(x, layer + 0.5f, y), btVector3(0.5f, 0.5f, 0.5f));
		layer++;
	};

	addTinyLayer();
	addBox(btVector3(0, layer + 0.5f, 0), btVector3(2.5f, 0.5f, 2.5f)); layer++;

	addBox(btVector3(-2, layer + 0.5f, 0), btVector3(.5f, 0.5f, 2.5f));
	addBox(btVector3(2, layer + 0.5f, 0), btVector3(.5f, 0.5f, 2.5f));
	for (int x = -1; x <= 1; x++)
		for (int y = -2; y <= 2; y++)
			addBox(btVector3(x, layer + 0.5f, y), btVector3(0.5f, 0.5f, 0.5f));
	layer++;

	addBox(btVector3(0, layer + 0.5f, 0), btVector3(2.5f, 0.5f, 2.5f)); layer++;

	addBox(btVector3(-3, layer + 0.5f, -3), btVector3(1.5f, 0.5f, 1.5f));
	addBox(btVector3(3, layer + 0.5f, -3), btVector3(1.5f, 0.5f, 1.5f));
	addBox(btVector3(3, layer + 0.5f, 3), btVector3(1.5f, 0.5f, 1.5f));
	addBox(btVector3(-3, layer + 0.5f, 3), btVector3(1.5f, 0.5f, 1.5f));

	addBox(btVector3(0, layer + 0.5f, 0), btVector3(1.5f, 0.5f, 1.5f));

	addBox(btVector3(0, layer + 0.5f, 2), btVector3(1.5f, 0.5f, 0.5f));
	addBox(btVector3(0, layer + 0.5f, -2), btVector3(1.5f, 0.5f, 0.5f));

	addBox(btVector3(2, layer + 0.5f, 0), btVector3(0.5f, 0.5f, 1.5f));
	addBox(btVector3(-2, layer + 0.5f, 0), btVector3(0.5f, 0.5f, 1.5f));


	layer++;
	addBox(btVector3(0, layer + 0.5f, 0), btVector3(2.5f, 0.5f, 2.5f)); layer++;


	//world->stepSimulation(10, 1000);

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
	world->addRigidBody(floorBody);
	floorBody->setFriction(1.0);

	boxes.push_back(new Box());
	boxes[boxes.size() - 1]->body = floorBody;
	boxes[boxes.size() - 1]->extends = extends;
}

void TowerTopple::update( float elapsedTime )
{
	world->stepSimulation((float)elapsedTime, 100);


	camDist += players[0]->joystick.leftStick.y * elapsedTime * 5;
	camAngle += players[0]->joystick.leftStick.x * elapsedTime * 90;


	if (players[0]->joystick.a == 1 && players[0]->prevJoystick.a == 0)
	{
		btSphereShape* groundShape = new btSphereShape(0.5f);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, 8, 10));

		float mass = 50;
		btVector3 fallInertia;
		groundShape->calculateLocalInertia(mass, fallInertia);

		btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo cInfo(mass, groundMotionState, groundShape, fallInertia);
		btRigidBody* floorBody = new btRigidBody(cInfo);
		world->addRigidBody(floorBody);
		floorBody->setFriction(1.0);
		floorBody->applyCentralImpulse(btVector3(0,0,-500));
	}

	for (auto p : players)
	{
		if (p->joystick.a == 1)
		{
			glm::vec4 mouse3d;
			glm::mat4 projectionMatrix = glm::perspective(90.0f, (float)settings->resX / settings->resY, 0.1f, 500.0f);
			glm::mat4 cameraMatrix;
			cameraMatrix = glm::rotate(cameraMatrix, -10.0f, glm::vec3(1, 0, 0));
			cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0, 0, -camDist));
			cameraMatrix = glm::rotate(cameraMatrix, camAngle, glm::vec3(1, 0, 0));
			renderer->unproject(settings->center, &mouse3d, &p->shootRay, cameraMatrix, projectionMatrix);
		}
	}


}

void TowerTopple::draw()
{
	glm::mat4 projectionMatrix = glm::perspective(90.0f, (float)settings->resX / settings->resY, 0.1f, 500.0f);
//	glm::mat4 cameraMatrix = glm::lookAt(glm::vec3(0, -5, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));


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


	float h = 0;
	std::vector<blib::VertexP3T2N3> verts;
	verts.push_back(blib::VertexP3T2N3(glm::vec3(-10, h, -10), glm::vec2(0, 0), glm::vec3(0, 1, 0)));
	verts.push_back(blib::VertexP3T2N3(glm::vec3(10, h, 10), glm::vec2(10, 10), glm::vec3(0, 1, 0)));
	verts.push_back(blib::VertexP3T2N3(glm::vec3(10, h, -10), glm::vec2(10, 0), glm::vec3(0, 1, 0)));

	verts.push_back(blib::VertexP3T2N3(glm::vec3(-10, h, -10), glm::vec2(0, 0), glm::vec3(0, 1, 0)));
	verts.push_back(blib::VertexP3T2N3(glm::vec3(10, h, 10), glm::vec2(10, 10), glm::vec3(0, 1, 0)));
	verts.push_back(blib::VertexP3T2N3(glm::vec3(-10, h, 10), glm::vec2(0, 10), glm::vec3(0, 1, 0)));

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
		
		btScalar m[16];
		b->body->getWorldTransform().getOpenGLMatrix(m);
		renderState.activeShader->setUniform(Uniforms::ModelMatrix, glm::scale(glm::make_mat4(m), glm::vec3(b->extends.x() * 2, b->extends.y() * 2, b->extends.z() * 2)));
		renderer->drawTriangles(boxVerts, renderState);


	}


	
	blib::RenderState r = debugDraw->renderstate;
	r.activeShader->setUniform(DebugDraw::Uniforms::ProjectionMatrix, projectionMatrix);
	r.activeShader->setUniform(DebugDraw::Uniforms::CameraMatrix, cameraMatrix);
	std::vector<blib::VertexP3C4> vertices;

	for (auto p : players)
	{
		vertices.push_back(blib::VertexP3C4(p->shootRay.origin, glm::vec4(1, 0, 0, 1)));
		vertices.push_back(blib::VertexP3C4(p->shootRay.origin + 100.0f * p->shootRay.dir, glm::vec4(1, 0, 0, 1)));
	}

	renderer->drawLines(vertices, r);





	world->debugDrawWorld();
	debugDraw->renderstate.activeFbo = spriteBatch->renderState.activeFbo;
	debugDraw->flush(renderer, projectionMatrix, cameraMatrix);


}

blib::Texture* TowerTopple::getTitleImage()
{
	return NULL;
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
