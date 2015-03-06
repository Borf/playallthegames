#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include <blib/math/Polygon.h>

#include "ZombieSurvivalPlayer.h"

namespace blib { class Texture; class Font; class Animation;  class VBO; class FBO; }

class ZombieSurvival : public AliveGame<ZombieSurvivalPlayer>
{
public:
	blib::Texture* backSprite;

	std::vector<blib::math::Polygon> objects;

	blib::FBO* visionFbo;




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