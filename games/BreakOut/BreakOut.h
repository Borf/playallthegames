#pragma once

#include "../../PlayAllTheGames/Game.h"

#include "BreakOutPlayer.h"

class BreakOutBall;

namespace blib { class Texture; class Font; };

class BreakOut : public Game<BreakOutPlayer>
{
	blib::Texture* pad;
	blib::Texture* block;
	blib::Texture* ball;
	std::vector<blib::Texture*> backgrounds;
	blib::Texture* back;
	blib::Texture* bigBack;
	blib::Font* font;


	glm::vec4 blocks[5][5];
	std::vector<BreakOutBall*> balls;
	float speed;
public:


	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual bool hasWinner();
	virtual std::list<Player*> getWinners();
	virtual blib::Texture* getTitleImage();
};

