#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include "FallDownPlayer.h"

namespace blib { class Texture; namespace math { class Rectangle; } };

class FallDown : public AliveGame<FallDownPlayer>
{
	blib::Texture* playerSprite;
	blib::Texture* blockSprite;
	blib::Texture* bladeSprite;
	blib::Texture* bladeBloodySprite;
	blib::Texture* backSprite;

	std::vector<std::vector<bool> > level;
	std::vector<bool> touchedBlades;

	float offset;
	float scrollBack;
	bool generateEmptyLine;

	int levelWidth;
	int levelHeight;

	float speed;

	bool hasCollision(const blib::math::Rectangle &r);

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

