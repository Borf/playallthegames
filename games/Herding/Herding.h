#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"

#include "HerdingPlayer.h"

namespace blib { class Texture; class Font; };

class Sheep;

class Herding : public ScoreGame<HerdingPlayer>
{
	blib::Texture* background;
	blib::Texture* playerSprite;
	blib::Texture* sheepSprite;
	blib::Texture* corner;
	blib::Font* font;

	void checkCollision(Sheep* sheep);
	bool checkCollision2(Sheep* sheep);

public:
	std::vector<Sheep*> sheep;

	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 4); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual blib::Texture* getTitleImage();
	virtual void update( float elapsedTime );
	virtual void draw();

};


