#pragma once

#include "../../playallthegames/AliveGame.h"

#include "AsteroidsPlayer.h"
#include "Asteroid.h"
namespace asteroids
{
	class Asteroids : public AliveGame<asteroids::Player>
	{
	public:

		blib::Texture* backgroundTexture;
		blib::Texture* playerTexture;
		blib::Texture* asteroidTexture[3];

		std::vector<Asteroid> asteroids;


		virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(99, 100); }
		std::string getName();
		virtual std::string getInstructions();
		virtual void loadResources();
		virtual void start(Difficulty difficulty);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual blib::Texture* getTitleImage();

	};
}