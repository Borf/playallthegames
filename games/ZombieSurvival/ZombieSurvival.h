#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include <blib/math/Polygon.h>
#include <blib/SpriteBatch.h>

#include "ZombieSurvivalPlayer.h"
#include "Zombie.h"

namespace blib { class Texture; class Font; class Animation;  class VBO; class FBO; }

class ZombieSurvival : public AliveGame<ZombieSurvivalPlayer>
{
public:
	blib::Texture* backSprite;
	blib::Texture* wallSprites;

	std::vector<blib::math::Polygon> objects;
	std::vector<blib::math::Polygon> collisionObjects;

	blib::FBO* visionFbo;
	blib::SpriteBatch::Cache* levelCache;


	std::vector<Zombie*> zombies;

	void spawnZombie();



	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};







extern "C" {
	__declspec( dllexport ) GameBase* getGame();
}