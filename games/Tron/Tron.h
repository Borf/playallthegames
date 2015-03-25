#pragma once

#include "../../PlayAllTheGames/AliveGame.h"

#include "TronPlayer.h"

#include <blib/RenderState.h>

namespace blib { class FBO; class SpriteBatch; class Texture; };

class Tron : public AliveGame<TronPlayer>
{
	std::vector<std::vector<bool> > screen;
	blib::FBO* fbo;
	blib::Texture* whitePixel;

	float speed;
public:
	Tron();
	virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
	std::string getName();
	virtual std::string getInstructions();
	virtual void loadResources();
	virtual void start(Difficulty difficulty);
	virtual void update( float elapsedTime );
	virtual void draw();
	virtual blib::Texture* getTitleImage();
};

