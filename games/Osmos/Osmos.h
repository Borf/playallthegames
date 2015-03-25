#pragma once

#include "OsmosPlayer.h"
#include "Sphere.h"
#include "../../PlayAllTheGames/Game.h"

class Osmos : public Game<OsmosPlayer>
{
public:
	blib::Texture* background;
	blib::Texture* ballTexture;
	std::vector<Sphere*> spheres;


	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual bool hasWinner();
	virtual std::list<Player*> getWinners();
	virtual blib::Texture* getTitleImage();

};

