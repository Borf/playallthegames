#pragma once

#include "../../PlayAllTheGames/AliveGame.h"

#include "ShowDownPlayer.h"

namespace blib { class Texture; class Font;  }

class ShowDown : public AliveGame<ShowDownPlayer>
{
	blib::Texture* backSprite;
	blib::Texture* trashSprite;
	blib::Texture* playerSprite;
	blib::Texture* playerOverlaySprite;
	blib::Texture* playerNormalSprite;
	blib::Texture* whitePixel;
	blib::Font* font;


	std::vector<std::pair<glm::vec3, glm::vec4> > gunshots;

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



