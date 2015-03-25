#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"

#include "VacuumSuckerPlayer.h"

namespace blib { class Texture; class Font;  }

class VacuumSucker : public ScoreGame<VacuumSuckerPlayer>
{
	blib::Texture* backSprite;
	blib::Texture* trashSprite;
	blib::Texture* playerSprite;
	blib::Font* font;

	int trashCount;
	int turningFactor;

	std::vector<glm::vec2> trash;

public:
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};



