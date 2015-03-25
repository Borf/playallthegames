#pragma once

#include "../../PlayAllTheGames/Game.h"

#include "ChickenPlayer.h"

namespace blib { class Texture; };

class Chicken : public Game<ChickenPlayer>
{
	blib::Texture* backSprite;
	blib::Texture* finalBackSprite;
	blib::Texture* carSprite;
	blib::Texture* carOverlaySprite;


	float scroll;
	float distance;
public:
	Chicken();
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

