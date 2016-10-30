#pragma once

#include "BugSwatPlayer.h"
#include "../../PlayAllTheGames/ScoreGame.h"

namespace blib { class Texture; class SpriteSheet; class Font; class AudioSample;  };

namespace bugswat
{
	class Splat;
	class Enemy;


	class BugSwat : public ScoreGame < BugSwatPlayer >
	{
		blib::SpriteSheet* swatterSprite;
		blib::SpriteSheet* flySprite;
		blib::Texture* back;
		blib::Texture* splatSprite;
		blib::Font* font;

		std::vector<Enemy*> enemies;
		std::vector<Splat*> splats;

		blib::AudioSample* swatSound;
		blib::AudioSample* hitSound;
		blib::AudioSample* flySound;

	public:
		BugSwat();
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