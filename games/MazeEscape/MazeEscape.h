#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"
#include <blib/math/Line.h>
#include "MazeEscapePlayer.h"

namespace blib { class Texture; class Font;  }

namespace mazeescape
{
	class Cell
	{
	public:
		bool visited = false;
		std::vector<std::pair<bool, Cell*> > neighbours;
	};

	class MazeEscape : public Game<MazeEscapePlayer>
	{
		blib::Texture* backSprite;
		blib::Texture* playerSprite;


	public:
		virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
		virtual std::string getName();
		virtual std::string getInstructions();
		virtual void loadResources();
		virtual void start(Difficulty difficulty);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual blib::Texture* getTitleImage();

		virtual bool hasWinner() override;
		virtual std::list<Player*> getWinners() override;


		std::vector<std::vector<Cell*> > maze;
		std::vector<blib::math::Line> lines;

	};

}

