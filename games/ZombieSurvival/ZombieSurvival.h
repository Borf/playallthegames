#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include <blib/math/Polygon.h>
#include <blib/SpriteBatch.h>

#include "ZombieSurvivalPlayer.h"
#include "Zombie.h"

namespace blib { class Texture; class Font; class Animation;  class VBO; class FBO; class Shader; }

class ZombieSurvival : public AliveGame<ZombieSurvivalPlayer>
{
public:
	blib::Texture* backSprite;
	blib::Texture* wallSprites;
	blib::Texture* finishSprite;
	blib::Shader* combineShader;
	enum ShaderAttributes
	{
		ProjectionMatrix,
		Matrix,
		s_texture,
		s_visionTexture,
		zombieFactor,
	};

	std::vector<blib::math::Polygon> objects;
	std::vector<blib::math::Polygon> collisionObjects;
	std::vector<blib::math::Rectangle> collisionAabb;
	std::vector<glm::ivec2> doors;
	

	blib::FBO* visionFbo;
	blib::SpriteBatch::Cache* levelCache;


	std::vector<Zombie*> zombies;

	void spawnZombie();



	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);

	void buildLevelCache();

	virtual void update(float elapsedTime);
	virtual void draw();
	virtual blib::Texture* getTitleImage();

/*	virtual bool hasWinner() override
	{
		return false; 
	}*/

};







extern "C" {
	__declspec( dllexport ) GameBase* getGame();
}