#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include <vector>

#include "FlyFarPlayer.h"

namespace blib { class Texture; };



class FlyFar : public Game<FlyFarPlayer>
{
	blib::Texture* flyerSprite;
	blib::Texture* rampSprite;
	blib::Texture* housesSprite;
	blib::Texture* gustSprite;
	blib::Texture* balloonSprite;
	std::vector<glm::vec2> gusts;
	std::vector<glm::vec2> balloons; 

	float lastScale;
	float lastLeft;

	float balloonChance;
	float gustFactor;


public:
	FlyFar();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();

	virtual bool hasWinner();

	virtual std::list<Player*> getWinners();

};

