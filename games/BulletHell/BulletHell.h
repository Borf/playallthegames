#pragma once

#include "../../PlayAllTheGames/AliveGame.h"

#include "BulletHellPlayer.h"

namespace blib { class Texture; class FBO; }

class Bullet;
class BulletGenerator;

class BulletHell : public AliveGame<BulletHellPlayer>
{
	blib::Texture* background;
	blib::Texture* shipSprite;
	blib::Texture* shipOverlaySprite;
	blib::Texture* bulletSprite;

	std::vector<Bullet*> bullets;
	std::vector<BulletGenerator*> bulletGenerators;

	float bulletSpawnDelay;
	float totalGameTime;
	float speed;
public:
	BulletHell();

	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();

};

