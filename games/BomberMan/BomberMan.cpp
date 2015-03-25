#include "BomberMan.h"

#include <blib/Texture.h>
#include <blib/ResourceManager.h>
#include <blib/SpriteSheet.h>
#include <blib/SpriteBatch.h>
#include <blib/Math.h>

#include "../../PlayAllTheGames/Participant.h"
#include "../../PlayAllTheGames/Settings.h"

#include "blocks/Block.h"
#include "blocks/WallBlock.h"
#include "blocks/DestructableBlock.h"
#include "blocks/EmptyBlock.h"
#include "blocks/powerups/BombPowerUp.h"
#include "blocks/powerups/FlamePowerUp.h"
#include "blocks/powerups/PushPowerUp.h"
#include "blocks/powerups/SpeedPowerUp.h"

#include "Bomb.h"


BomberMan::BomberMan()
{
	levelHeight = 13;
	levelWidth = 19;

}


std::string BomberMan::getName()
{
	return "BomberMan";
}

std::string BomberMan::getInstructions()
{
	return "Place bombs and explode other people";
}


void BomberMan::loadResources()
{
	playerSprite =		resourceManager->getResource<blib::Texture>("assets/games/BomberMan/player.png");
	bombSprite =		resourceManager->getResource<blib::Texture>("assets/games/BomberMan/bomb.png");
	blocksSheet =		resourceManager->getResource<blib::SpriteSheet>("assets/games/BomberMan/tiles");
	explosionParticleSprite = resourceManager->getResource<blib::Texture>("assets/games/BomberMan/explosion.png");
}

void BomberMan::start(Difficulty difficulty)
{
	blockSize = glm::min(settings->resX/levelWidth, settings->resY/levelHeight);

	blib::linq::deleteall(bombs);
	//blib::linq::deleteall(explosionParticles);
	explosionParticles.clear();

	blib::linq::foreach(level, blib::linq::deleteall<std::vector<Block*> >);
	level.clear();
	for (int y = 0; y < levelHeight; y++)
	{
		std::vector<Block*> row;
		for (int x = 0; x < levelWidth; x++)
		{
			if (y == 0 || x == 0 || y == levelHeight - 1 || x == levelWidth - 1 || (y%2 == 0 && x%2 == 0))
				row.push_back(new WallBlock(glm::vec2(blockSize*x, blockSize*y), blockSize));
			else if (blib::math::randomDouble() < 0.8f)
				row.push_back(new DestructableBlock(glm::vec2(blockSize*x, blockSize*y), blockSize));
			else
				row.push_back(new EmptyBlock(glm::vec2(blockSize*x, blockSize*y), blockSize));
		}
		level.push_back(row);
	}

	int spacing = (int)pow(2, 5-((int)players.size()-1)/2);

	for(auto p : players)
		p->position = glm::vec2(blockSize * (p->index % 4 < 2 ? 1 + spacing * (p->index / 4) : levelWidth - 2 - spacing * (p->index / 4)), 
		p->index % 2 == 0 ? (1+2*((p->index/4)%2))*blockSize : (levelHeight - 2-2*((p->index/4)%2))*blockSize);

	for(auto p : players)
	{
		for (int x = -1; x <= 1; x++)
			for (int y = -1; y <= 1; y++)
				if(level[(int)(p->position.y/blockSize) + y][(int)(p->position.x/blockSize) + x]->canExplode())
				{
					delete level[(int)(p->position.y / blockSize) + y][(int)(p->position.x / blockSize) + x];
					level[(int)(p->position.y / blockSize) + y][(int)(p->position.x / blockSize) + x] = new EmptyBlock(glm::vec2(p->position.x + blockSize * x, p->position.y + blockSize * y), blockSize);
				}
	}

	gameTime = 0;

	bombTimer = 2;

}


bool BomberMan::hasCollision(glm::vec2 position, glm::vec2 oldPosition)
{
	blib::math::Rectangle playerRect(position, blockSize, blockSize);
	playerRect.inflate(-1,-1);
	for(auto row : level)
		for(auto block : row)
			if(block->blocks() && block->rect.intersect(playerRect))
				return true;
	if(blib::linq::any(bombs, [this,position, oldPosition] (Bomb* b) { return glm::length(position - b->position) < (int)(0.9 * blockSize) && glm::length(oldPosition - b->position) > (int)(0.9 * blockSize); }))
		return true;
	return false;
}

void BomberMan::update( float elapsedTime )
{
	gameTime += elapsedTime;
	for(auto p : players)
	{
		if(!p->alive)
			continue;
		//movement
		glm::vec2 oldPos = p->position;

		glm::vec2 dir = p->joystick.leftStick;

		float length = glm::length(dir);
		if (length > 0.01f)
		{
			if (glm::abs(dir.x) > glm::abs(dir.y))
				dir.y = 0;
			else
				dir.x = 0;
			if (dir.x != 0 || dir.y != 0)
				dir = glm::normalize(dir);
			dir = dir*length;

			for (int count = 0; count < p->speed; count++)
			{
				std::vector<glm::vec2> newPositions;


				if (!hasCollision(p->position + dir, p->position))
				{
					p->position += dir * elapsedTime * 60.0f;
				}
				else if (dir.x != 0 || dir.y != 0)
				{
					glm::vec2 dir2(0,0);
					if (dir.x == 0)
					{
						float tileX = ((p->position.x + blockSize) / (2 * blockSize)) - (int)glm::floor((p->position.x + blockSize) / (2 * blockSize));
						if (tileX > 0.5)
							dir2 = glm::vec2(1,0);
						else if (tileX < 0.5)
							dir2 = glm::vec2(-1, 0);
					}
					else if (dir.y == 0)
					{
						float tileY = ((p->position.y + blockSize) / (2 * blockSize)) - (int)glm::floor((p->position.y + blockSize) / (2 * blockSize));
						if (tileY > 0.5)
							dir2 = glm::vec2(0, 1);
						else if (tileY < 0.5)
							dir2 = glm::vec2(0, -1);
					}
					dir2 *= length;

					if (!hasCollision(p->position + dir2 * elapsedTime * 60.0f, p->position))
					{
						p->position += dir2 * elapsedTime * 60.0f;
					}

				}
			}
		}
		//kickin' bombs
		
		
		if (p->pushBombs)
		{
			for(auto b : bombs)
			{
				if(glm::length(p->position - b->position) < blockSize && glm::length(oldPos-b->position) >= blockSize)
				{
					glm::vec2 diff = (b->position - p->position);
					if (glm::abs(diff.x) > glm::abs(diff.y))
						diff.y = 0;
					else
						diff.x = 0;
					diff = glm::normalize(diff);
					b->movement = (blockSize/4.0f) * diff;
				}
			}
		}



		//item pickups
		blib::math::Rectangle playerRect(p->position, playerSprite->originalWidth, playerSprite->originalHeight);
		for (size_t y = 0; y < level.size(); y++)
		{
			for (size_t x = 0; x < level[y].size(); x++)
			{
				if(level[y][x]->rect.intersect(playerRect))
				{
					if(level[y][x]->onTouch(p))
					{
						delete level[y][x];
						level[y][x] = new EmptyBlock(glm::vec2(x * blockSize, y * blockSize), blockSize);
					}
				}
			}
		}

		//bomb placement
		if(p->joystick.a == 1 && p->prevJoystick.a == 0 && blib::linq::count(bombs, [p] (Bomb* b) { return b->owner == p; }) < p->bombs)
		{
			int tileX = (int) glm::floor((p->position.x+32)/blockSize);
			int tileY = (int) glm::floor((p->position.y+32)/blockSize);
			bombs.push_back(new Bomb(glm::vec2(tileX*blockSize, tileY*blockSize), gameTime, p));
		}
	}

	for(int i = (int)bombs.size()-1; i >= 0; i--)
	{
		glm::vec2 oldPos = bombs[i]->position;
		bombs[i]->position += bombs[i]->movement * elapsedTime * 60.0f;
		if (hasCollision(bombs[i]->position, oldPos))
		{
			bombs[i]->position = oldPos;
			bombs[i]->movement = glm::vec2(0,0);
		}

		if (gameTime - bombs[i]->timePlaced > bombTimer)
		{
			glm::vec2 directions[] = { glm::vec2(-1,0), glm::vec2(1,0), glm::vec2(0,-1), glm::vec2(0,1) };

			for(auto direction : directions)
			{
				int length = bombs[i]->owner->flameLength;
				glm::ivec2 p((int)glm::round(bombs[i]->position.x / blockSize), (int)glm::round(bombs[i]->position.y / blockSize));
				for (int ii = 1; ii <= length; ii++)
				{
					glm::ivec2 p2(p.x + ii * (int)direction.x, p.y + ii * (int)direction.y);

					for(auto b : bombs)
						if(b->position == glm::vec2(blockSize*p2.x, blockSize*p2.y))
							b->timePlaced-=10;

					if(level[p2.y][p2.x]->blocks())
					{
						if (level[p2.y][p2.x]->canExplode())
						{
							delete level[p2.y][p2.x];
							double d = blib::math::randomDouble();
							if(d < 0.075)
								level[p2.y][p2.x] = new BombPowerUp(glm::vec2(blockSize * p2.x, blockSize * p2.y), blockSize);
							else if (d < 0.15)
								level[p2.y][p2.x] = new SpeedPowerUp(glm::vec2(blockSize * p2.x, blockSize * p2.y), blockSize);
							else if (d < 0.225)
								level[p2.y][p2.x] = new FlamePowerUp(glm::vec2(blockSize * p2.x, blockSize * p2.y), blockSize);
							else if (d < 0.3)
								level[p2.y][p2.x] = new PushPowerUp(glm::vec2(blockSize * p2.x, blockSize * p2.y), blockSize);
							else
								level[p2.y][p2.x] = new EmptyBlock(glm::vec2(blockSize * p2.x, blockSize * p2.y), blockSize);

							length = ii;
						}
						else
							length = ii - 1;
						break;
					}

				}

				for (int ii = 0; ii < blockSize * length; ii += 4)
					explosionParticles.push_back(glm::vec3(bombs[i]->position + (float)ii * direction, blib::math::randomFloat(0.7f, 1.0f)));
			}

			bombs.erase(bombs.begin()+i);
		}
	}


	for (std::list<glm::vec3>::iterator it = explosionParticles.begin(); it != explosionParticles.end(); it++)
	{
		for(auto b : bombs)
			if(glm::length(b->position - glm::vec2(it->x, it->y)) < blockSize)
				b->timePlaced-=10;

		if(it->z > 0.3f)
			for(auto p : players)
				if(glm::length(p->position - glm::vec2(it->x, it->y)) < 0.75 * blockSize)
					p->alive = false;
		it->z -= 0.025f * elapsedTime * 60;
	}

	explosionParticles.remove_if([] (glm::vec3 p) { return p.z < 0; });






}

void BomberMan::draw()
{
	spriteBatch->begin();
	for(auto row : level) for(auto block : row) block->draw(spriteBatch, blocksSheet);

	float scale = blockSize / (float)playerSprite->originalHeight;
	for(auto p : players)
		if(p->alive)
			spriteBatch->draw(playerSprite, blib::math::easyMatrix(p->position,0,glm::vec2(scale,scale)), p->participant->color);
	scale = blockSize / (float)bombSprite->originalHeight;
	for(auto b : bombs)
		spriteBatch->draw(bombSprite, blib::math::easyMatrix(b->position,0,glm::vec2(scale,scale)));
	spriteBatch->end();


	spriteBatch->renderState.dstBlendColor = blib::RenderState::ONE;
	spriteBatch->renderState.dstBlendAlpha = blib::RenderState::ONE;
	spriteBatch->begin();

	scale = blockSize / (float)explosionParticleSprite->originalHeight;
	for(auto p : explosionParticles)
		spriteBatch->draw(explosionParticleSprite, blib::math::easyMatrix(glm::vec2(p.x, p.y), 0, glm::vec2(scale,scale)), glm::vec4(1,1,1,p.z));

	spriteBatch->end();

	spriteBatch->renderState.dstBlendColor = blib::RenderState::ONE_MINUS_SRC_ALPHA;
	spriteBatch->renderState.dstBlendAlpha = blib::RenderState::ONE_MINUS_SRC_ALPHA;

}

blib::Texture* BomberMan::getTitleImage()
{
	return NULL;
}
