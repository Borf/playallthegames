#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"
#include "PokerStepsPlayer.h"

namespace blib { class Texture; class Font; }

class PokerSteps : public ScoreGame<PokerStepsPlayer>
{
	blib::Texture* backSprite;
	blib::Texture* playerSprite;
	blib::Texture* buttonsSprite;
	blib::Font* font;

	enum State
	{
		VOTING,
		SHOWING,
		WALKING,
		WON,
	} state;

	float stateStart;


public:
	PokerSteps();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(4, 4); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};


