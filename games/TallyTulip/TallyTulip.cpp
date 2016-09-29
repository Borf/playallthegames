#include "TallyTulip.h"
#include <blib/SpriteSheet.h>
#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/Math.h>
#include <blib/linq.h>

#include "../../playallthegames/Participant.h"

namespace tallytulip
{
	std::string TallyTulip::getName()
	{
		return "TallyTulip";
	}

	std::string TallyTulip::getInstructions()
	{
		return "Count the number of purple/blue tulips";
	}

	std::pair<int, int> TallyTulip::getPlayerCount()
	{
		return std::pair<int, int>(2, 999);
	}

	void TallyTulip::loadResources()
	{
		back = resourceManager->getResource<blib::Texture>("assets/games/TallyTulip/back.png");
		playerSprite = resourceManager->getResource<blib::Texture>("assets/games/TallyTulip/player.png");
		tulip = resourceManager->getResource<blib::SpriteSheet>("assets/games/TallyTulip/plant");
		tulip2 = resourceManager->getResource<blib::SpriteSheet>("assets/games/TallyTulip/plant2");
		font = resourceManager->getResource<blib::Font>("lindsey");
	}

	void TallyTulip::start()
	{
		tulips.clear();

		float fac = blib::math::randomFloat(0.05, 0.075);

		for (int i = 0; i < 1000; i++)
		{
			Tulip t;
			t.pos = glm::vec2(blib::math::randomFloat(0, 5120), blib::math::randomFloat(100, 1080-150));
			t.time = 0;
			t.inc = blib::math::randomFloat(0.5, 1);
			t.color = blib::math::randomFloat() < fac ? 1 : 0;
			t.offset = blib::math::randomFloat(200, 1000);
			t.scale = blib::math::randomFloat(0.8f, 1.0f);
			if (blib::math::randomFloat() > 0.5f)
				t.scale = -t.scale;
			tulips.push_back(t);
		}
		std::sort(tulips.begin(), tulips.end(), [](const Tulip &a, const Tulip &b) { return a.pos.y < b.pos.y;  });
		time = -1920;

		speed = 5;
	}

	void TallyTulip::update(float elapsedTime)
	{
		time += elapsedTime * 100 * speed;
		for (auto &t : tulips)
		{
			if(t.pos.x < 1920+time-t.offset)
			t.time += elapsedTime*t.inc;
		}

		for (auto p : players)
		{
			if (p->joystick.a != 0 && p->prevJoystick.a == 0 && time < 5120)
			{
				p->jump = 20;
				p->count++;
			}
			if (p->joystick.b != 0 && p->prevJoystick.b == 0 && time < 5120)
			{
				p->jump = 20;
				p->count--;
			}

			p->jump = .9 * p->jump;
		}
	}
	void TallyTulip::draw()
	{
		spriteBatch->begin();

		float ttt = time;
		while (ttt > 1920)
			ttt -= 1920;
		while (ttt < 0)
			ttt += 1920;

		spriteBatch->draw(back, blib::math::easyMatrix(glm::vec2(-ttt, 0)));
		spriteBatch->draw(back, blib::math::easyMatrix(glm::vec2(-ttt+1920, 0)));

		int i = 0;
		for (const auto &t : tulips)
		{
			int index = 17 - (int)(t.time* 10 * speed);
			if (index < 0)
				index = 0;

			blib::SpriteSheet* s = t.color == 0 ? tulip : tulip2;

			spriteBatch->draw(s->getFrame(index % 6, index / 6), blib::math::easyMatrix(t.pos - glm::vec2(time,0), 0.0f, glm::vec2(t.scale, abs(t.scale))), s->spriteCenter);
			i++;
		}

		for (auto p : players)
		{
			spriteBatch->draw(playerSprite, blib::math::easyMatrix(glm::vec2(100 + 100 * p->index, 1000 - p->jump)), p->participant->color);
		}

		if(time > 5120)
		{ 
			int count = blib::linq::count(tulips, [](const Tulip& t) { return t.color == 1; });

			spriteBatch->draw(font, "Correct amount: " + std::to_string(count), blib::math::easyMatrix(glm::vec2(800, 700)));
			for (auto p : players)
			{
				spriteBatch->draw(font, std::to_string(p->count), blib::math::easyMatrix(glm::vec2(1600, 600 +50 * p->index)), p->participant->color);
			}

		}

		spriteBatch->end();
	}

	blib::Texture* TallyTulip::getTitleImage()
	{
		return nullptr;
	}

	bool TallyTulip::hasWinner()
	{
		return time > 5500;
	}

	std::list<Player*> TallyTulip::getWinners()
	{
		int count = blib::linq::count(tulips, [](const Tulip& t) { return t.color == 1; });
		int minDist = blib::linq::min<int>(players, [count](TallyTulipPlayer* p) { return p->count - count; });
		return blib::linq::where<std::list<Player*>>(players, [count, minDist](TallyTulipPlayer* p) { return (p->count - count) == minDist; });
	}

}