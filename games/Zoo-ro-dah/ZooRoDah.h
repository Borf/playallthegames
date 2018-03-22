#pragma once

#include "../../PlayAllTheGames/ScoreGame.h"

#include "ZooRoDahPlayer.h"

namespace blib { class Texture; class Font; class SpriteSheet; }

namespace zoorodah
{
	class ZooRoDah : public ScoreGame<ZooRoDahPlayer>
	{
		blib::Texture* backSprite;
		blib::Texture* trashSprite;
		blib::Texture* playerSprite;
		blib::Font* font;

		std::vector<glm::vec3> positions;

		blib::SpriteSheet* elephantSprite;
		blib::SpriteSheet* giraffeSprite;
		blib::SpriteSheet* pandaSprite;

		std::vector<glm::vec2> spawns;
	public:
		ZooRoDah();
		virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 1); }
		virtual std::string getName();
		virtual std::string getInstructions();
		virtual void loadResources();
		virtual void start();
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual blib::Texture* getTitleImage();
	};

}

