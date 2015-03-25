#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include "SoccerPlayer.h"
#include <Box2D/Box2D.h>

class b2World;
class b2Body;
class b2Joint;

namespace blib { class Texture; }

class Soccer : public AliveGame <SoccerPlayer>, b2ContactListener
{
	blib::Texture* backgroundTexture;
	blib::Texture* playerSprite;
	blib::Texture* player1Sprite;
	blib::Texture* player2Sprite;
	blib::Texture* ballSprite;
	blib::Texture* bottleSprite;
	blib::Texture* circleSprite;

	b2World* world;
	float physTime;

	b2Body* ball;
	b2Joint* possessionJoint;
	b2Body* possessionPlayer;

public:
	Soccer();
	virtual std::string getName() override;
	virtual std::string getInstructions() override;
	virtual std::pair<int, int> getPlayerCount() override;
	virtual void loadResources() override;
	virtual void start(Difficulty difficulty) override;
	virtual void update(float elapsedTime) override;
	virtual void draw() override;
	virtual blib::Texture* getTitleImage() override;

	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;

};

