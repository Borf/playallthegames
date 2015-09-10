#pragma once

#include "../../playallthegames/AliveGame.h"

#include "AsteroidsPlayer.h"

namespace asteroids
{

	class Asteroids : public AliveGame<asteroids::Player>
	{

		blib::Texture* backgroundTexture;
		blib::Texture* playerTexture;

		virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
		std::string getName();
		virtual std::string getInstructions();
		virtual void loadResources();
		virtual void start(Difficulty difficulty);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual blib::Texture* getTitleImage();

	};
}