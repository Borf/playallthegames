#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include "MicroRacePlayer.h"

namespace blib { class Texture; }
class Car;

class MicroRace : public AliveGame<MicroRacePlayer>
{
	blib::Texture* carSprite;
	blib::Texture* back;
	blib::Texture* lineSprite;
	blib::Texture* footSprite;

	std::vector<Car*> cars;
	float speed;
	float backPosition;
	float spawnFactor;
	float gameTime;
public:
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 8); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
	void scrollBackground(float elapsedTime);
	void spawnCar( float gameTime );
	void drawBackground();
	void drawLines();
};


