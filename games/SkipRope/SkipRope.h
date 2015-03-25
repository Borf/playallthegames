#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include "SkipRopePlayer.h"

namespace blib { class Texture; };


class SkipRope : public AliveGame<SkipRopePlayer>
{
	blib::Texture* playerSprite;
	blib::Texture* playerBackSprite;
	blib::Texture* backSprite;
	blib::Texture* handSprite;

	int jumpCount;
	float frame;
	float speed;
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

