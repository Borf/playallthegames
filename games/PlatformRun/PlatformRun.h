#pragma once

#include "../../PlayAllTheGames/AliveGame.h"

#include "PlatformRunPlayer.h"

namespace blib { class Texture; class Font;  }

namespace platformrun
{

	class Level;

	class PlatformRun : public AliveGame<PlatformRunPlayer>
	{
		blib::Texture* backgroundTexture;
		blib::Texture* backgroundFrontTexture;
		blib::Texture* playerSprite;

		float scrollOffset = 0;
		float scrollSpeed;
		Level* level = nullptr;
	public:
		PlatformRun();
		virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
		virtual std::string getName();
		virtual std::string getInstructions();
		virtual void loadResources();
		virtual void start();
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual blib::Texture* getTitleImage();
	};


}
