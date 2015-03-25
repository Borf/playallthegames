#include "BreakOut.h"
#include <blib/linq.h>
#include <blib/SpriteBatch.h>
#include <blib/Texture.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/util/Profiler.h>
#include <blib/Math.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#include "BreakOutBall.h"


std::string BreakOut::getName()
{
	return "BreakOut";
}

std::string BreakOut::getInstructions()
{
	return "Break all the blocks and bounce the ball";
}


void BreakOut::loadResources()
{
	pad =	resourceManager->getResource<blib::Texture>("assets/games/BreakOut/pad.png");
	block =	resourceManager->getResource<blib::Texture>("assets/games/BreakOut/block.png");
	ball =	resourceManager->getResource<blib::Texture>("assets/games/BreakOut/ball.png");
	bigBack=resourceManager->getResource<blib::Texture>("assets/games/BreakOut/back.png");
	font =	resourceManager->getResource<blib::Font>("lindsey");

	int i = 1;
	while(true)
	{
		blib::Texture* b = resourceManager->getResource<blib::Texture>("assets/games/BreakOut/back" + blib::util::toString(i) + ".png");
		if(b->loaded)
			backgrounds.push_back(b);
		else
			break;
		i++;
	}

}

void BreakOut::start(Difficulty difficulty)
{
	for(int x = 0; x < 5; x++)
		for(int y = 0; y < 5; y++)
			blocks[x][y] = glm::vec4(x/10.0f, y/10.0f, 0.5f, 1.0f);
	back = backgrounds[rand() % backgrounds.size()];

	for(auto p : players)
		p->position = (float)(1920/2-players.size()*50 + 100*p->index);

	blib::linq::deleteall(balls);
	for(size_t i = 0; i < players.size(); i++)
		balls.push_back(new BreakOutBall(glm::vec2(players[i]->position, 950)));
	for(auto p : players)
		p->ball = balls[p->index];
	speed = 1;
}

void BreakOut::update( float elapsedTime )
{
	speed += 60 * elapsedTime * 0.002f;
	for(auto p : players)
	{
		p->position = glm::min(1920.0f, glm::max(0.0f, p->position + 10.0f * p->joystick.leftStick.x * elapsedTime * 60.0f));
	}


	int i = 0;
	for(auto b : balls)
	{
		b->position += (1 + ((speed - 1) / 5.0f)) * 6 * b->direction * elapsedTime * 60.0f;
		
		if((b->position.x < 0 && b->direction.x < 0) || (b->position.x > 1980-ball->originalWidth && b->direction.x > 0))
			b->direction.x = -b->direction.x;
		if(b->position.y < 0 && b->direction.y < 0)
			b->direction.y = -b->direction.y;

		if (b->position.y > 1000 - ball->originalHeight && b->position.y < 1000 - ball->originalHeight + pad->originalHeight && b->direction.y > 0)
		{
			if (b->position.x >= players[i]->position - ball->originalWidth && b->position.x <= players[i]->position + pad->originalWidth)
			{
				b->direction.y = -b->direction.y;
				float dx = ((b->position.x + ball->originalWidth / 2) - (players[i]->position + pad->originalWidth / 2)) / (pad->originalWidth / 2);
				b->direction.x = glm::clamp(b->direction.x + dx,-1.0f,1.0f);
			}
		}


		blib::math::Rectangle r(b->position, ball->originalWidth, ball->originalHeight);
		for(int x = 0; x < 5; x++)
		{
			for(int y = 0; y < 5; y++)
			{
				if(blocks[x][y].a < 1)
					continue;
				blib::math::Rectangle r2(200.0f + 300 * x, 200.0f + 60 * y, (float)block->originalWidth, (float)block->originalHeight);
				if(r2.intersect(r))
				{
					players[i]->score++;
					blocks[x][y].a = 0;

					//balls[i] -= 5 * ballDirs[i];
					glm::vec2 diff = (r.topleft + r.size()/2.0f) - (r2.topleft + r2.size()/2.0f);

					if (glm::abs(glm::abs(diff.y) - 30) < 10)
						b->direction.y = -b->direction.y;
					if (glm::abs(glm::abs(diff.x) - 150) < 10)
						b->direction.x = -b->direction.x;
				}
			}
		}
		i++;
	}

}

void BreakOut::draw()
{
	spriteBatch->begin(settings->scaleMatrix);
	spriteBatch->draw(bigBack, glm::mat4(), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), glm::vec4(glm::sin(blib::util::Profiler::getAppTime()/4)*0.5f+0.5f, glm::sin(blib::util::Profiler::getAppTime()/7)*0.5f+0.5f, glm::sin(blib::util::Profiler::getAppTime()/13)*0.5f+0.5f,1));
	spriteBatch->draw(back, blib::math::easyMatrix(glm::vec2(200,200)));


	for(auto p : players)
	{
		for(int i = 0; i < 3; i++)
			spriteBatch->draw(pad, blib::math::easyMatrix(glm::vec2(p->position, 1000)), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), p->participant->color * glm::vec4(1,1,1,0.33333f));
		spriteBatch->draw(ball, blib::math::easyMatrix(balls[p->index]->position), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), p->participant->color);
		spriteBatch->draw(font, blib::util::toString(p->score), blib::math::easyMatrix(glm::vec2(10.0f, 50.0f*p->index)), p->participant->color);
	}




	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			spriteBatch->draw(block, blib::math::easyMatrix(glm::vec2(200 + 300 * x, 200 + 60 * y)), glm::vec2(0,0), blib::math::Rectangle(0,0,1,1), blocks[x][y]);
	spriteBatch->end();
}

bool BreakOut::hasWinner()
{
	int aliveCount = blib::linq::count(balls, [] (BreakOutBall* b) { return b->isInGame(); } );
	if(aliveCount == 0)
		return true;
	if(aliveCount == 1)
		if(blib::linq::max<int, BreakOutPlayer*>(players, [] (BreakOutPlayer* p) { return p->score; }, [] (BreakOutPlayer* p) { return p; })->ball->isInGame())
			return true;
	return false;
}

std::list<Player*> BreakOut::getWinners()
{
	int aliveCount = blib::linq::count(balls, [] (BreakOutBall* b) { return b->isInGame(); } );
	if(aliveCount == 1)
		return blib::linq::where<std::list<Player*>>(players, [] (BreakOutPlayer* p) { return p->ball->isInGame(); });

			
	int maxScore = blib::linq::max<int>(players, [] (BreakOutPlayer* p) { return p->score; });
	return blib::linq::where<std::list<Player*>>(players, [maxScore] (BreakOutPlayer* p) { return p->score == maxScore; });
}

blib::Texture* BreakOut::getTitleImage()
{
	return NULL;
}
