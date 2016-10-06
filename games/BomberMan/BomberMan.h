#pragma once

#include "../../PlayAllTheGames/AliveGame.h"

#include "BombermanPlayer.h"


class Block;
class Bomb;
class ExplosionParticle;

namespace blib { class Texture; class SpriteSheet; class AudioSample;  }

class BomberMan : public AliveGame<BomberManPlayer>
{
	std::vector<std::vector<Block*> > level;
	std::list<glm::vec3> explosionParticles;
	std::vector<Bomb*> bombs;

	int levelWidth;
	int levelHeight;
	int blockSize;
	float gameTime;


	float bombTimer;

	blib::Texture* playerSprite;
	blib::Texture* bombSprite;
	blib::SpriteSheet* blocksSheet;
	blib::Texture* explosionParticleSprite;

	bool hasCollision(glm::vec2 position, glm::vec2 oldPosition);
public:

	blib::AudioSample* explosionSound;
	blib::AudioSample* dropSound;
	blib::AudioSample* kickSound;
	blib::AudioSample* dieSound;
	blib::AudioSample* pickupSound;

	BomberMan();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 8); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start();
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};


