#pragma once

#include "../../playallthegames/ScoreGame.h"

#include "Player.h"

namespace blib { class Font; class Texture; }

namespace nipplerub
{
	class NippleRub : public ScoreGame<Player>
	{
		blib::Texture* tex;
		blib::Texture* finger;
		blib::Texture* fingerOverlay;
		blib::Texture* whitePixel;
		blib::Font* font;
	public:


		NippleRub();
		virtual std::string getName() override;
		virtual std::string getInstructions() override;
		virtual std::pair<int, int> getPlayerCount() override;
		virtual void loadResources() override;
		virtual void start(Difficulty difficulty) override;
		virtual void update(float elapsedTime) override;
		virtual void draw() override;
		virtual blib::Texture* getTitleImage() override;
	};


}