#pragma once

#include "../../PlayAllTheGames/AliveGame.h"

#include "CurveFeverPlayer.h"

#include <blib/RenderState.h>

namespace blib { class FBO; class SpriteBatch; class Texture; };

class CurveFever : public AliveGame<CurveFeverPlayer>
{
	std::vector<std::vector<int> > screen;
	blib::FBO* fbo;
	blib::Texture* whitePixel;
	blib::Texture* head;
	blib::Texture* direction;

	float speed;
	double gameTime;
public:
	CurveFever();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};

