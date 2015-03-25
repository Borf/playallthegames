#pragma once

#include "TanksPlayer.h"
#include "../../PlayAllTheGames/AliveGame.h"


namespace blib {
	class Texture; 	class ParticleSystem;
};

class b2World;
class b2Body;


class Tanks : public AliveGame<TanksPlayer>
{
	blib::ParticleSystem* particleSystem;

	blib::Texture* back;
	blib::Texture* tankSprite;
	blib::Texture* turretSprite;
	blib::Texture* bulletSprite;
	blib::Texture* crateSprite;
	b2World* world;

	std::vector<b2Body*> bullets;
	std::vector<b2Body*> crates;

public:
	Tanks();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};

