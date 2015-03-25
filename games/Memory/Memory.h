#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"
#include "MemoryPlayer.h"

class Tile;
class RotatingCard;
namespace blib { class Texture; class SpriteSheet; }

class Memory : public ScoreGame<MemoryPlayer>
{
	std::vector<blib::SpriteSheet*> tileSheets;
	blib::Texture* border;
	blib::Texture* cursor;
	blib::Texture* selectionSprite;
	blib::Texture* backSprite;
	blib::SpriteSheet* tileSheet;


	int width;
	int height;
	std::vector<std::vector<Tile*> > tiles;
	std::vector<RotatingCard*> rotatingCards;
	bool started;
	float gameTime;
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

