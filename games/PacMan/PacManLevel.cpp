#include "PacManLevel.h"
#include <blib/SpriteBatch.h>
#include <blib/SpriteSheet.h>
#include <blib/Math.h>
#include <blib/Color.h>
#include <blib/gl/Vertex.h>

namespace pacman
{
	bool PacManLevel::isWalkable( int x, int y, const std::vector<Enemy*> &enemies, float gameTime, Enemy* enemy /*= NULL*/ )
	{
		return levelData[y][x] == 0 || levelData[y][x] == 2;
	}

	PacManLevel::PacManLevel()
	{
		levelCache = NULL;
		pelletCache = NULL;
	}

	std::vector<std::vector<int> > createMask(int a, int b, int c, int d, int e, int f, int g, int h, int i)
	{
		std::vector<std::vector<int> > ret;

		std::vector<int> l1,l2,l3;
		l1.push_back(a); l1.push_back(b); l1.push_back(c);
		l2.push_back(d); l2.push_back(e); l2.push_back(f);
		l3.push_back(g); l3.push_back(h); l3.push_back(i);
		ret.push_back(l1);
		ret.push_back(l2);
		ret.push_back(l3);
		return ret;
	}

	struct ltstr
	{
		bool operator()(const glm::ivec2& s1, const glm::ivec2& s2) const
		{
			if(s1.x != s2.x)
				return s1.x < s2.x;
			else
				return s1.y < s2.y;

		}
	};


	void PacManLevel::draw( blib::SpriteBatch* spriteBatch )
	{
		if(levelCache == NULL)
		{
			std::map<glm::ivec2, std::vector<std::vector<int> >,ltstr> masksDouble;
			std::map<glm::ivec2, std::vector<std::vector<int> >, ltstr> masks;
			masksDouble[glm::ivec2(1,0)] = createMask( // = boven
				1, 1, 1 ,
				1, 1, 1 ,
				0, 2, 0);
			masksDouble[glm::ivec2(1,5)] = createMask( // = onder
				0, 2, 0 ,
				1, 1, 1 ,
				1, 1, 1);
			masksDouble[glm::ivec2(0,1)] = createMask( // || links
				1, 1, 0 ,
				1, 1, 2 ,
				1, 1, 0);
			masksDouble[glm::ivec2(5,1)] = createMask( // || rechts
				0, 1, 1 ,
				2, 1, 1 ,
				0, 1, 1);
		
			masksDouble[glm::ivec2(2,0)] = createMask( // top
				2, 2, 2 ,
				1, 1, 3 ,
				2, 1, 1);
			masksDouble[glm::ivec2(4,0)] = createMask(
				2, 2, 2 ,
				3, 1, 1 ,
				1, 1, 2);

			masksDouble[glm::ivec2(0,2)] = createMask( // left
				2, 1, 2 ,
				2, 1, 1 ,
				2, 3, 1);
			masksDouble[glm::ivec2(0,4)] = createMask(
				2, 3, 1 ,
				2, 1, 1 ,
				2, 1, 2);

			masksDouble[glm::ivec2(5,2)] = createMask( // right
				2, 1, 2 ,
				1, 1, 2 ,
				1, 3, 2);
			masksDouble[glm::ivec2(5,4)] = createMask(
				1, 3, 2 ,
				1, 1, 2 ,
				2, 1, 2);

			masksDouble[glm::ivec2(0,0)] = createMask(	0, 1, 1 ,// ╔
													0, 1, 1 ,
													0, 1, 2);
			masksDouble[glm::ivec2(5,0)] = createMask(	1, 1, 0 ,// ╗
													1, 1, 0 ,
													2, 1, 0);
			masksDouble[glm::ivec2(0,5)] = createMask(	0, 1, 2 , // ╚
													0, 1, 1 ,
													0, 1, 1);
			masksDouble[glm::ivec2(5,5)] = createMask(	2, 1, 0 ,// ╝
													1, 1, 0 ,
													1, 1, 0);
		//small corners
			masksDouble[glm::ivec2(6,2)] = createMask(	2, 2, 2 ,
													2, 1, 1 ,
													2, 1, 1);
			masksDouble[glm::ivec2(7,2)] = createMask(	2, 2, 2 ,
													1, 1, 2 ,
													1, 1, 2);
			masksDouble[glm::ivec2(6,3)] = createMask(	2, 1, 1 ,
													2, 1, 1 ,
													2, 2, 2);
			masksDouble[glm::ivec2(7,3)] = createMask(	1, 1, 2 ,
													1, 1, 2 ,
													2, 2, 2);


			masks[glm::ivec2(2,1)] = createMask(0, 1, 1 ,
												2, 1, 1 ,
												0, 1, 1);
			masks[glm::ivec2(4,1)] = createMask(1, 1, 0 ,
												1, 1, 2 ,
												1, 1, 0);
			masks[glm::ivec2(1,2)] = createMask(0, 2, 0 ,
												1, 1, 1 ,
												1, 1, 1);
			masks[glm::ivec2(1,4)] = createMask(1, 1, 1 ,
												1, 1, 1 ,
												0, 2, 0);
			masks[glm::ivec2(2,2)] = createMask(0, 2, 0 ,
												1, 1, 2 ,
												0, 1, 0);
			masks[glm::ivec2(2,4)] = createMask(0, 1, 0 ,
												1, 1, 2 ,
												0, 2, 0);
			masks[glm::ivec2(4,4)] = createMask(0, 1, 0 ,
												2, 1, 1 ,
												0, 2, 0);
			masks[glm::ivec2(4,2)] = createMask(0, 2, 0 ,
												2, 1, 1 ,
												0, 1, 0);

			masks[glm::ivec2(6,0)] = createMask(1, 1, 1 ,
												1, 1, 1 ,
												1, 1, 2);
			masks[glm::ivec2(7,0)] = createMask(1, 1, 1 ,
												1, 1, 1 ,
												2, 1, 1);
			masks[glm::ivec2(6,1)] = createMask(1, 1, 2 ,
												1, 1, 1 ,
												1, 1, 1);
			masks[glm::ivec2(7,1)] = createMask(2, 1, 1 ,
												1, 1, 1 ,
												1, 1, 1);

			spriteBatch->startCache();
			for(size_t y = 0; y < levelData.size(); y++)
			{
				for(size_t x = 0; x < levelData[y].size(); x++)
				{
					bool doubleLine = (levelData[y][x] & 4) != 0;
					if (!doubleLine && (x == 0 || y == 0 || x == levelData[y].size() - 1 || y == levelData.size() - 1))
						continue;

					int checkValue = levelData[y][x];

					for(auto entry : doubleLine ? masksDouble : masks)
					{
						bool match = true;
						for (int yy = 0; yy < 3; yy++)
						{
							for (int xx = 0; xx < 3; xx++)
							{
								int xxx = x + xx - 1;
								int yyy = y + yy - 1;
								if (xxx < 0 || xxx >= (int)levelData[0].size() || yyy < 0 || yyy >= (int)levelData.size())
								{
									if (entry.second[yy][xx] == 3 || entry.second[yy][xx] == 2)
										match = false;
									continue;
								}
								if (entry.second[yy][xx] == 1 && (levelData[yyy][xxx] & 3) != 1)
									match = false;
								if (entry.second[yy][xx] == 3 && levelData[yyy][xxx] != checkValue)
									match = false;
								if (entry.second[yy][xx] == 2 && ((levelData[yyy][xxx] & 3) != 0 && (levelData[yyy][xxx] & 3) != 2))
									match = false;
							}
						}
						if (match)
						{
							spriteBatch->draw(backgroundSheet->getFrame(entry.first.x, entry.first.y), blib::math::easyMatrix(glm::vec2(32 * x, 32 * y)), glm::vec2(0,0), blib::Color::blue);
							break;
						}
					}
				}
			}
			levelCache = spriteBatch->getCache();
		}
		else
			spriteBatch->drawCache(levelCache);

		if(pelletCache == NULL)
		{
			spriteBatch->startCache();
			for(size_t y = 0; y < levelData.size(); y++)
				for(size_t x = 0; x < levelData[y].size(); x++)
					if(levelData[y][x] == 2)
						spriteBatch->draw(backgroundSheet->getFrame(1,1), blib::math::easyMatrix(glm::vec2(32*x, 32*y)));
			pelletCache = spriteBatch->getCache();
		}
		else
			spriteBatch->drawCache(pelletCache);

	}

	bool PacManLevel::tryEat( int x, int y )
	{
		if (levelData[y][x] == 2)
		{
			levelData[y][x] = 0;
			delete pelletCache;
			pelletCache = NULL;
			return true;
		}
		return false;
	}

}