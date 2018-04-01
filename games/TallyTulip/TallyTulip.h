#pragma once

#include "../../playallthegames/AliveGame.h"
#include "TallyTulipPlayer.h"

namespace blib { class SpriteSheet; class Font; }

namespace tallytulip
{
	class Tulip
	{
	public:
		glm::vec2 pos;
		float time;
		float inc;
		int color;
		float offset;
		float scale;
	};


	class TallyTulip : public Game<TallyTulipPlayer>
	{
		blib::SpriteSheet* tulip;
		blib::SpriteSheet* tulip2;
		blib::Texture* back;
		blib::Texture* playerSprite;
		blib::Font* font;
		std::vector<Tulip> tulips;

		float speed;
		int timeLimit;

	public:
		float time;


		virtual std::string getName() override;
		virtual std::string getInstructions() override;
		virtual std::pair<int, int> getPlayerCount() override;
		virtual void loadResources() override;
		virtual void start() override;
		virtual void update(float elapsedTime) override;
		virtual void draw() override;
		virtual blib::Texture* getTitleImage() override;
		virtual bool hasWinner() override;
		virtual std::list<Player*> getWinners() override;

	};
}