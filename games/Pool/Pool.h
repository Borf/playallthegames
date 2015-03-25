#pragma once

#include "PoolPlayer.h"
#include "../../PlayAllTheGames/ScoreGame.h"
#include <vector>

#include <Box2D/Box2D.h>

class b2World;
class b2Body;

namespace blib { class Texture; class Font;  }


class Pool : public ScoreGame<PoolPlayer>, b2ContactListener
{
	b2World* world;

	float scale = 80;
	float pocketSize = 1.25f;


	blib::Font* font;
	blib::Texture* pocketSprite;
	blib::Texture* ballSprite;
	blib::Texture* halfBallSprite;
	blib::Texture* cueSprite;
	blib::Texture* whitePixel;

	std::vector<std::pair<b2Body*, PoolPlayer*> > balls;
	std::vector<glm::vec2> holes;

public:
	Pool();

	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1,100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update(float elapsedTime);
	virtual void draw();
	virtual blib::Texture* getTitleImage();
	std::vector<glm::vec2> addHole(const std::vector<glm::vec2> &verts, glm::vec2 pos, float radius);

	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

};

