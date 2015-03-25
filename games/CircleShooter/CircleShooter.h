#pragma once

#include "../../PlayAllTheGames/AliveGame.h"

#include "CircleShooterPlayer.h"

namespace blib { class Texture; class Font; }

class Bullet
{
public:
	glm::vec2 position;
	glm::vec2 inc;
};

class CircleShooter : public AliveGame < CircleShooterPlayer >
{
	blib::Texture* playerSprite;
	blib::Texture* bubbleSprite;
	blib::Texture* bulletSprite;
	blib::Texture* backSprite;

	float shootTimer;
	int index;


	glm::vec2 getPos(double d) const;
	std::vector<Bullet> bullets;
	float spawnTimer;


public:
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	virtual std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update(float elapsedTime);
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};

