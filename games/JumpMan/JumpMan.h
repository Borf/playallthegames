#pragma once

#include "../../PlayAllTheGames/AliveGame.h"
#include "JumpManPlayer.h"

namespace jumpman
{
	class Enemy;


	class JumpMan : public AliveGame < JumpManPlayer >
	{
		blib::Texture* sky;
		blib::Texture* playerSprite;
		blib::Texture* grass;
		blib::Texture* enemySprite;
		blib::Texture* wheelSprite;

		Enemy* enemy;

		int grassHeight;

		float speed;
	public:


		virtual std::pair<int, int> getPlayerCount() { return std::pair<int, int>(1, 100); }
		virtual std::string getName();
		virtual std::string getInstructions();
		virtual void loadResources();
		virtual void start(Difficulty difficulty);
		virtual void update(float elapsedTime);
		virtual void draw();
		virtual blib::Texture* getTitleImage();
	};

}