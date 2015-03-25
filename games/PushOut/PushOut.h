#pragma once

#include "../../PlayAllTheGames/AliveGame.h"

#include "PushOutPlayer.h"

class b2World;

class PushOut : public AliveGame<PushOutPlayer>
{
	blib::Texture* background;
	blib::Texture* playerSprite;
	b2World* world;
public:
	PushOut();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();

	float physTime;

};

