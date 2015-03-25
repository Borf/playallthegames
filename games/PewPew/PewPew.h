#pragma once

#include "PewPewPlayer.h"
#include "../../PlayAllTheGames/ScoreGame.h"
#include <vector>

#include <Box2D/Box2D.h>

class b2World;
class b2Body;
class Level;

namespace blib { class Texture; class Font;  }


class PewPew : public Game<PewPewPlayer>
{
	b2World* world;
	float time;

	Level* level;

	blib::Texture* playerSprite;

public:
	PewPew();

	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update(float elapsedTime);
	virtual void draw();
	virtual blib::Texture* getTitleImage();
	virtual bool hasWinner() override;

	virtual std::list<Player*> getWinners() override;

};

