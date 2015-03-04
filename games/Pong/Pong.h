#pragma once

#include "../../ScoreGame.h"

#include "PongPlayer.h"

namespace blib { class Texture; class Font; }

class Pong : public ScoreGame<PongPlayer>
{
	blib::Texture* backSprite;
	blib::Texture* trashSprite;
	blib::Texture* playerSprite;
	blib::Font* font;

	boolean positive;
	int trashCount;
	int turningFactor;
	int speed;

	std::vector<glm::vec2> trash;

public:
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update(float elapsedTime);
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};







extern "C" {
	__declspec(dllexport) GameBase* getGame();
}