#include "MacroMachines.h"
#include "Wheel.h"

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/User.h"

#include <blib/json.h>

#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/ResourceManager.h>
#include <blib/Box2DDebug.h>
#include <blib/RenderState.h>
#include <blib/FBO.h>
#include <blib/Renderer.h>
#include <blib/LineBatch.h>
#include <blib/Color.h>
#include <blib/math/HermiteCurve.h>
#include <blib/Math.h>
#include <blib/Util.h>
#include <blib/util/Log.h>
#include <blib/util/Profiler.h>
#include <blib/util/FileSystem.h>

#include <Direct.h>


#include <glm/gtc/matrix_transform.hpp>
#include <Box2D/Box2D.h>
#define _USE_MATH_DEFINES
#include <math.h>

using blib::util::Log;


MacroMachines::MacroMachines()
{
	world = NULL;
}

std::string MacroMachines::getName()
{
	return "Macro Machines";
}

std::string MacroMachines::getInstructions()
{
	return "Finish First";
}

void MacroMachines::loadResources()
{
	backgroundSprite = resourceManager->getResource<blib::Texture>("assets/games/MacroMachines/back.png");
	asphaltSprite = resourceManager->getResource<blib::Texture>("assets/games/MacroMachines/asphalt.png");
	carSprite = resourceManager->getResource<blib::Texture>("assets/games/MacroMachines/car.png");
	slipSprite = resourceManager->getResource<blib::Texture>("assets/games/MacroMachines/slip.png");
	finishSprite = resourceManager->getResource<blib::Texture>("assets/games/MacroMachines/finish.png");
	font = resourceManager->getResource<blib::Font>("tahoma");

	asphaltSprite->setTextureRepeat(true);

	blib::Box2DDebug::getInstance()->init(lineBatch, renderer);
}




void MacroMachines::start(Difficulty difficulty)
{
	zoom = 10 * settings->scale;
	lastPhysicsTick = 0;
	this->maxPlayerScore = 5;

	if (world)
		delete world;
	world = new b2World(b2Vec2(0, 0));

	std::string difficultyStr = "normal";
	switch (difficulty)
	{
	case Difficulty::Easy:		difficultyStr = "easy";		break;
	case Difficulty::Normal:	difficultyStr = "normal";	break;
	case Difficulty::Hard:		difficultyStr = "hard";		break;
	case Difficulty::Cruel:		difficultyStr = "cruel";	break;
	case Difficulty::Wtf:		difficultyStr = "wtf";		break;
	}

	std::vector<std::string> tracks = blib::util::FileSystem::getFileList("assets/games/MacroMachines/tracks/" + difficultyStr);
	for (size_t i = tracks.size(); i > 0; i--)
	{
		if (tracks[i - 1][tracks[i - 1].size() - 1] == '/')
		{
			tracks.erase(tracks.begin() + i - 1);
			continue;
		}
	}
	blib::json::Value track = blib::util::FileSystem::getJson("assets/games/MacroMachines/tracks/"+difficultyStr+"/" + tracks[rand() % tracks.size()]);

	if (track.isNull())
		track = blib::util::FileSystem::getJson("assets/games/MacroMachines/tracks/default.json");
	if (blib::util::FileSystem::exists("assets/games/MacroMachines/tracks/override.json"))
		track = blib::util::FileSystem::getJson("assets/games/MacroMachines/tracks/override.json");


	std::vector<b2Vec2> verts;
	std::vector<b2Vec2> verts2;
	std::vector<blib::VertexP2T2C4> triangles;

	progressLines.clear();


	for (size_t i = 0; i < track["nodes"].size(); i++)
	{
		int ii = (i + 1) % track["nodes"].size();
		glm::vec2 n1 = blib::util::fromAngle(glm::radians(track["nodes"][i]["angle"].asFloat()));
		glm::vec2 n2 = blib::util::fromAngle(glm::radians(track["nodes"][ii]["angle"].asFloat()));
		glm::vec2 p1(track["nodes"][i]["pos"][0u].asFloat(), track["nodes"][i]["pos"][1u].asFloat());
		glm::vec2 p2(track["nodes"][ii]["pos"][0u].asFloat(), track["nodes"][ii]["pos"][1u].asFloat());

		blib::math::BiArc left(p1, n1, p2, -n2);
		left.setOffset(track["width"].asFloat() / 2);
		blib::math::BiArc right(p1, n1, p2, -n2);
		right.setOffset(track["width"].asFloat() / -2);

		float len = glm::max(left.length(), right.length());

		float inc = 1 / (len / 1);


		b2BodyDef progressBodyDef;
		b2Body* progressBody = world->CreateBody(&progressBodyDef);
		b2ChainShape shape;
		b2Vec2 v[2] = { left.getPointLinear(0), right.getPointLinear(0) };
		shape.CreateChain(v, 2);
		b2FixtureDef progressFixtureDef;
		progressFixtureDef.shape = &shape;
		progressFixtureDef.isSensor = true;
		progressBody->CreateFixture(&progressFixtureDef);
		progressLines.push_back(progressBody);


		for (float f = 0; f < 1; f += inc)
		{				
			verts.push_back(left.getPoint(f));
			verts2.push_back(right.getPoint(f));

			triangles.push_back(blib::VertexP2T2C4(left.getPointLinear(f),			0.052f * left.getPointLinear(f), blib::Color::white));
			triangles.push_back(blib::VertexP2T2C4(left.getPointLinear(f + inc),	0.052f * left.getPointLinear(f + inc), blib::Color::white));
			triangles.push_back(blib::VertexP2T2C4(right.getPointLinear(f),			0.052f * right.getPointLinear(f), blib::Color::white));

			triangles.push_back(blib::VertexP2T2C4(left.getPointLinear(f + inc),	0.052f * left.getPointLinear(f + inc), blib::Color::white));
			triangles.push_back(blib::VertexP2T2C4(right.getPointLinear(f + inc),	0.052f * right.getPointLinear(f + inc), blib::Color::white));
			triangles.push_back(blib::VertexP2T2C4(right.getPointLinear(f),			0.052f * right.getPointLinear(f), blib::Color::white));
		}
	}



	int last = track["nodes"].size() - 1;
	glm::vec2 n1 = blib::util::fromAngle(glm::radians(track["nodes"][0]["angle"].asFloat()));
	glm::vec2 n2 = blib::util::fromAngle(glm::radians(track["nodes"][last]["angle"].asFloat()));
	glm::vec2 p1(track["nodes"][0]["pos"][0u].asFloat(), track["nodes"][0]["pos"][1u].asFloat());
	glm::vec2 p2(track["nodes"][last]["pos"][0u].asFloat(), track["nodes"][last]["pos"][1u].asFloat());
	road = blib::math::BiArc(p1, -n1, p2, n2);

	float f = 0.1f;
	for (auto p : players)
	{
		road.setOffset(p->index%2 == 0 ? -2.0f : 2.0f);
		glm::vec2 prevPos = road.getPointLinear(f-0.01f);
		glm::vec2 pos = road.getPointLinear(f);
		if (p->index%2 == 1)
			f += 0.15f;

		float angle = atan2(pos.y-prevPos.y, pos.x-prevPos.x);

		p->spawn(world, pos, MacroMachinesPlayer::Drive::FWD, angle-blib::math::pif/2);
	}



	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, 0.0f);
	b2Body* body = world->CreateBody(&groundBodyDef);

	b2ChainShape chain;
	chain.CreateLoop(&verts[0], verts.size());

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chain;
	body->CreateFixture(&fixtureDef);
	
	if (!verts2.empty())
	{
		body = world->CreateBody(&groundBodyDef);
		b2ChainShape chain2;
		chain2.CreateLoop(&verts2[0], verts2.size());
		fixtureDef.shape = &chain2;
		body->CreateFixture(&fixtureDef);
	}

	world->SetDebugDraw(blib::Box2DDebug::getInstance());
	blib::Box2DDebug::getInstance()->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit | b2Draw::e_pairBit);



	trackSprite = resourceManager->getResource<blib::FBO>();
	trackSprite->setSize(settings->resX, settings->resY);

	glm::mat4 tx;
	tx = glm::translate(tx, glm::vec3(settings->center, 0));
	tx = glm::scale(tx, glm::vec3(zoom, -zoom, 1));

	blib::RenderState rs = spriteBatch->renderState;
	rs.activeVbo = NULL;
	rs.activeTexture[0] = asphaltSprite;
	rs.activeShader->setUniform(blib::SpriteBatch::ShaderAttributes::Matrix, tx);
	rs.activeFbo = trackSprite;

	renderer->drawTriangles(triangles, rs);


	world->SetContactListener(this);

}

void MacroMachines::update(float elapsedTime)
{
	glm::mat4 tx;
	tx = glm::translate(tx, glm::vec3(settings->center, 0));
	tx = glm::scale(tx, glm::vec3(zoom, -zoom, 1));

	spriteBatch->renderState.srcBlendAlpha = blib::RenderState::ONE;

	spriteBatch->begin(tx, trackSprite);


	int iterations = 0;
	lastPhysicsTick += elapsedTime;
	while (lastPhysicsTick > 1 / 100.0f)
	{
		world->Step(1 / 100.0f, 5, 5);


		for (auto car : players)
		{
			car->forward = car->joystick.a != 0;
			car->braking = car->joystick.b != 0;

			car->steering = 0.5f * car->joystick.leftStick.x;
			for (auto w : car->wheels)
			{
				float slipFactor = (w->getKillVelocityVector() - w->body->GetLinearVelocity()).Length();
				if (slipFactor > 10)
				{
					float fac = glm::clamp((slipFactor - 7) / 9, 0.0f, 1.0f);
					if ((w->body->GetPosition() - w->lastSlipSpot).Length() < 1)
						spriteBatch->draw(slipSprite,
						blib::math::easyMatrix(w->body->GetPosition(), glm::degrees(w->body->GetAngle()), glm::vec2(0.02f, 0.02f)), slipSprite->center, glm::vec4(1, 1, 1, fac));
					w->lastSlipSpot = w->body->GetPosition();
				}
			}
			car->update(elapsedTime);
		}





		lastPhysicsTick -= 1 / 100.0f;
		iterations++;
	}




	spriteBatch->end();
	spriteBatch->renderState.srcBlendAlpha = blib::RenderState::SRC_ALPHA;


}

void MacroMachines::draw()
{
	glm::mat4 tx;
	tx = glm::translate(tx, glm::vec3(settings->center, 0));
	tx = glm::scale(tx, glm::vec3(zoom,zoom,1));


	spriteBatch->begin();

	spriteBatch->draw(backgroundSprite, glm::mat4());
	spriteBatch->draw(trackSprite, glm::mat4());


	spriteBatch->end();


	spriteBatch->begin(tx);

	glm::vec2 diff = ((b2ChainShape*)progressLines[0]->GetFixtureList()->GetShape())->m_vertices[0] - ((b2ChainShape*)progressLines[0]->GetFixtureList()->GetShape())->m_vertices[1];
	glm::vec2 center = 0.5f * (((b2ChainShape*)progressLines[0]->GetFixtureList()->GetShape())->m_vertices[1] + ((b2ChainShape*)progressLines[0]->GetFixtureList()->GetShape())->m_vertices[0]);
	float angle = atan2(diff.y, diff.x);
	float len = glm::length(diff);


	spriteBatch->draw(finishSprite, blib::math::easyMatrix(center, glm::degrees(angle), len / glm::vec2(finishSprite->originalWidth, finishSprite->originalWidth)), finishSprite->center);


	for (auto car : players)
	{
		spriteBatch->draw(carSprite, blib::math::easyMatrix(car->body->GetPosition(), glm::degrees(car->body->GetAngle()), glm::vec2(2.0f / carSprite->originalWidth, 4.0f / carSprite->originalHeight)), carSprite->center, car->participant->color);
		spriteBatch->draw(font, blib::util::toString(car->score), blib::math::easyMatrix(car->body->GetPosition() + glm::vec2(-0.15f, -0.15f), 0, glm::vec2(0.25f, 0.25f)), blib::Color::black);
		spriteBatch->draw(font, blib::util::toString(car->score), blib::math::easyMatrix(car->body->GetPosition() + glm::vec2(-0.15f, 0.15f), 0, glm::vec2(0.25f, 0.25f)), blib::Color::black);
		spriteBatch->draw(font, blib::util::toString(car->score), blib::math::easyMatrix(car->body->GetPosition() + glm::vec2(0.15f, 0.15f), 0, glm::vec2(0.25f, 0.25f)), blib::Color::black);
		spriteBatch->draw(font, blib::util::toString(car->score), blib::math::easyMatrix(car->body->GetPosition() + glm::vec2(0.15f, -0.15f), 0, glm::vec2(0.25f, 0.25f)), blib::Color::black);
		spriteBatch->draw(font, blib::util::toString(car->score), blib::math::easyMatrix(car->body->GetPosition(), 0, glm::vec2(0.25f, 0.25f)));
	}


	spriteBatch->end();


#if 0
	lineBatch->begin(tx);


/*	for (size_t i = 0; i < track.size(); i++)
	{
		int ii = (i + 1) % track.size();
		//lineBatch->draw(blib::math::ArcPath(track[i].pos, track[i].direction, track[ii].pos), glm::vec4(1,0,0,1));
		glm::vec2 n1 = glm::normalize(glm::vec2(track[i].direction.y, -track[i].direction.x));
		glm::vec2 n2 = glm::normalize(glm::vec2(track[ii].direction.y, -track[ii].direction.x));

		lineBatch->draw(blib::math::BiArc(track[i].pos + 7.5f * n1, glm::normalize(track[i].direction), track[ii].pos + 7.5f * n2, -glm::normalize(track[ii].direction)));
		lineBatch->draw(blib::math::BiArc(track[i].pos - 7.5f * n1, glm::normalize(track[i].direction), track[ii].pos - 7.5f * n2, -glm::normalize(track[ii].direction)));
		//break;
	}*/

	lineBatch->draw(road);

	world->DrawDebugData();
	lineBatch->end();
#endif

}

blib::Texture* MacroMachines::getTitleImage()
{
	return NULL;
}

void MacroMachines::BeginContact(b2Contact* contact)
{
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	MacroMachinesPlayer* player = NULL;
	int progressLine = -1;

	for (size_t i = 0; i < players.size(); i++)
	{
		if (players[i]->body == bodyA || players[i]->body == bodyB)
			player = players[i];
	}
	for (size_t i = 0; i < progressLines.size(); i++)
	{
		if (progressLines[i] == bodyA || progressLines[i] == bodyB)
			progressLine = i;
	}

	if (player && progressLine != -1)
	{
		if (progressLine == (player->levelIndex + 1) % progressLines.size())
		{
			player->levelIndex = progressLine;
			if (progressLine == 0)
			{
				player->score++;
			}
		}
		Log::out << "Player " << player->participant->user->name << " hit " << progressLine << Log::newline;
	}






}

