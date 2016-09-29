#pragma once

#include "../../playallthegames/AliveGame.h"

namespace blib { class ParticleSystem; class AudioSample; }

#include "AsteroidsPlayer.h"
#include "Asteroid.h"
namespace asteroids
{
	class Asteroids : public AliveGame<asteroids::Player>
	{
	public:
		blib::ParticleSystem* particleSystem;
		blib::Texture* backgroundTexture;
		blib::Texture* playerTexture;
		blib::Texture* asteroidTexture[3];
		blib::Texture* bulletTexture;

		blib::AudioSample* laser;
		blib::AudioSample* explosion;

		std::vector<Asteroid> asteroids;

		std::vector<std::pair<glm::vec4, asteroids::Player*> > bullets;

		virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
		std::string getName();
		virtual std::string getInstructions();
		virtual void loadResources();
		virtual void start();
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual blib::Texture* getTitleImage();

	};
}