#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"

#include "BattlePlayer.h"

namespace blib { class Texture; class SpriteSheet; class Font; };
class BattleEnemy;

class Battle : public ScoreGame<BattlePlayer>
{
	blib::SpriteSheet* levelTexture;
	blib::Texture* playerTexture;
	blib::Texture* enemyTexture;
	blib::Texture* enemyMaskTexture;
	blib::Font* font;
	BattleLevel* level;
	std::vector<BattleEnemy*> enemies;
public:

	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 8); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};






