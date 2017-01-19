#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"

#include "CheckersPlayer.h"
#include <blib/math/Polygon.h>

namespace blib { class Texture; class Font; }

namespace checkers
{
	class Tile
	{
	public:
		CheckersPlayer* player;
		blib::math::Polygon polygon;
		Tile(CheckersPlayer* player, const blib::math::Polygon &polygon)
		{
			this->player = player;
			this->polygon = polygon;
		}
	};

	class Checkers : public ScoreGame<CheckersPlayer>
	{
	public:
		std::vector<std::vector<std::vector<Tile>>> tiles;


		blib::Texture* whitePixel;
		blib::Texture* stoneSprite;
		blib::Texture* cursorSprite;

		Checkers();
		virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(100, 100); }
		virtual std::string getName();
		virtual std::string getInstructions();
		virtual void loadResources();
		virtual void start();
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual blib::Texture* getTitleImage();
	};


}
