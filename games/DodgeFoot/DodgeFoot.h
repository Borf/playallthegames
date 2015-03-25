#pragma once

#include "DodgeFootPlayer.h"

#include "../../PlayAllTheGames/AliveGame.h"
namespace blib { class Texture; };

class DodgeFoot : public AliveGame<DodgeFootPlayer>
{
	blib::Texture* playerSprite;
	blib::Texture* floorSprite;
	blib::Texture* footSprite;
	blib::Texture* redFootSprite;
	blib::Texture* footMaskSprite;
	blib::Texture* skySprite;

	glm::vec2 footPosition;
	float targetFootLocation;
	bool footMovingDown;

//	TimeSpan? screenShake = null;
	float screenShake;
	float lastHit;
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

