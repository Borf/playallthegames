#include "Checkers.h"

#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>
#include <blib/math/Line.h>
#include <blib/math/Polygon.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace checkers
{



	Checkers::Checkers()
	{
	}

	std::string Checkers::getName()
	{
		return "Checkers";
	}

	std::string Checkers::getInstructions()
	{
		return "Check mate?";
	}

	void Checkers::loadResources()
	{
		whitePixel = resourceManager->getResource<blib::Texture>("assets/textures/whitepixel.png");
		stoneSprite = resourceManager->getResource<blib::Texture>("assets/games/Checkers/stone.png");
		cursorSprite = resourceManager->getResource<blib::Texture>("assets/cursor.png");
	}

	void Checkers::start()
	{
		float s1 = 300;
		float s2 = 300;

		float chunkSize = 2 * glm::pi<float>() / players.size();

		for (size_t i = 0; i < players.size(); i++)
		{
			players[i]->cursor = 200.0f * blib::math::fromAngle(blib::math::pif/players.size() +  blib::math::pif * 2 / players.size() * i);


			float a1 = chunkSize * i;
			float a2 = chunkSize * (i + 1);

			float a3 = (a1 + a2) / 2;


			blib::math::Line l3(s1 * blib::math::fromAngle(a1), s1 * blib::math::fromAngle(a1) + s2 * blib::math::fromAngle(a3));
			blib::math::Line l4(s1 * blib::math::fromAngle(a2), s1 * blib::math::fromAngle(a2) + s2 * blib::math::fromAngle(a3));

			blib::math::Line lt(s1 * blib::math::fromAngle(a1) + s2 * blib::math::fromAngle(a3), s1 * blib::math::fromAngle(a2) + s2 * blib::math::fromAngle(a3));
			blib::math::Line l7(glm::vec2(0, 0), lt.mix(0.5f));

			std::vector<blib::math::Line> lines;

			for (float f = 0; f <= 1; f += 0.25f)
			{
				lines.push_back(blib::math::Line(l4.mix(f), l7.mix(f)));
				lines.push_back(blib::math::Line(l7.mix(f), l3.mix(f)));
			}

			std::vector<std::vector<Tile>> board;

			for (int x = 0; x < 4; x++)
			{
				std::vector<Tile> row;
				for (int y = 0; y < 4; y++)
				{

					int index = y * 2;
					float off = 0;

					if (x / 2 == 1)
						index += 1;
					if (x % 2 == 1)
						off += 0.5f;

					row.push_back(Tile(y >= 2 && (x+y)%2==0 ? players[i] : nullptr, blib::math::Polygon({
						lines[index].mix(off), 
						lines[index].mix(off + 0.5), 
						lines[index + 2].mix(off + 0.5),
						lines[index + 2].mix(off) })));
				}
				board.push_back(row);
			}
			tiles.push_back(board);
		}

	}

	void Checkers::update(float elapsedTime)
	{
		for (auto p : players)
		{
			p->cursor += 400.0f * p->joystick.leftStick * elapsedTime;
			p->cursor = glm::clamp(p->cursor, glm::vec2(-960, -540), glm::vec2(960, 540));

			if (p->joystick.b != 0 && p->prevJoystick.b == 0)
				p->pickedPiece = -1;

			if (p->joystick.a != 0 && p->prevJoystick.a == 0)
			{
				if (p->pickedPiece < 0)
				{
					for (size_t i = 0; i < p->pieces.size(); i++)
					{
						if (tiles[p->pieces[i].z][p->pieces[i].x][p->pieces[i].y].polygon.contains(p->cursor))
						{
							p->pickedPiece = i;
							break;
						}
					}
				}
				else
				{
					glm::ivec4 &pos = p->pieces[p->pickedPiece];
					//other algorithm: generate possible targets based on position. jumping over is quite complicated...

					glm::ivec4 target(0, 0, 0, -1);

					if (pos.z == p->index) //player is on own board, can only move forward diagonally
					{
						if (pos.y > 0) //hasn't reached the edge yet
						{
							glm::ivec2 p1(pos.x - 1, pos.y - 1);
							if (p1.x >= 0)
								if (tiles[pos.z][p1.x][p1.y].polygon.contains(p->cursor))
									target = glm::vec4(p1.x, p1.y, pos.z, pos.w);
							glm::ivec2 p2(pos.x + 1, pos.y - 1);
							if (p2.x < 4)
								if (tiles[pos.z][p2.x][p2.y].polygon.contains(p->cursor))
									target = glm::vec4(p2.x, p2.y, pos.z, pos.w);

						} 
						else
						{// reached the edge, let's see where it can go to
							if (pos.x == 0)
							{//can only jump to the centerthingy
								if (tiles[(pos.z + 1) % players.size()][2][0].polygon.contains(p->cursor))
									target = glm::ivec4(2, 0, (pos.z + 1) % players.size(), 0);
							}
							if (pos.x == 2)
							{//can jump to any center thingy
								for(size_t i = 0; i < players.size(); i++)
									if(i != p->index)
										if (tiles[i][2][0].polygon.contains(p->cursor))
											target = glm::ivec4(2, 0, i, 0);
							}
						}
					}
					else
					{	//player is on another board, can only move 'backwards'
						glm::ivec2 p1(pos.x - 1, pos.y + 1);
						if (p1.x >= 0 && p1.y < 4)
							if (tiles[pos.z][p1.x][p1.y].polygon.contains(p->cursor))
								target = glm::vec4(p1.x, p1.y, pos.z, pos.w);
						glm::ivec2 p2(pos.x + 1, pos.y + 1);
						if (p2.x < 4 && p1.y < 4)
							if (tiles[pos.z][p2.x][p2.y].polygon.contains(p->cursor))
								target = glm::vec4(p2.x, p2.y, pos.z, pos.w);
						if (pos.x == 2 && pos.y == 0) //special
							if (tiles[pos.z-1][0][0].polygon.contains(p->cursor))
								target = glm::vec4(0, 0, pos.z-1, pos.w);
					}

					if (target.w > -1)
					{
						p->pieces[p->pickedPiece] = target;
						p->pickedPiece = -1;
					}
				}
			}

		}


	}

	void Checkers::draw()
	{
		glm::mat4 transform = blib::math::easyMatrix(glm::vec2(1920 / 2, 1080 / 2));
		lineBatch->begin(transform, 5.0f);
		spriteBatch->begin(transform);

		for (auto &board : tiles)
		{
			for (int x = 0; x < 4; x++)
			{
				for (int y = 0; y < 4; y++)
				{
					blib::math::Polygon& p = board[x][y].polygon;

					std::vector<std::pair<glm::vec2, glm::vec2>> coords;

					coords.push_back(std::pair<glm::vec2, glm::vec2>(p[0], glm::vec2(0, 0)));
					coords.push_back(std::pair<glm::vec2, glm::vec2>(p[1], glm::vec2(0, 0)));
					coords.push_back(std::pair<glm::vec2, glm::vec2>(p[2], glm::vec2(0, 0)));

					coords.push_back(std::pair<glm::vec2, glm::vec2>(p[0], glm::vec2(0, 0)));
					coords.push_back(std::pair<glm::vec2, glm::vec2>(p[2], glm::vec2(0, 0)));
					coords.push_back(std::pair<glm::vec2, glm::vec2>(p[3], glm::vec2(0, 0)));

					spriteBatch->draw(whitePixel, glm::mat4(), coords, (x + y) % 2 == 0 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));
				}
			}
		}


		for (auto p : players)
		{
			for (size_t i = 0; i < p->pieces.size(); i++)
			{
				glm::ivec4 &t = p->pieces[i];
				
				if (i == p->pickedPiece)
				{
					spriteBatch->draw(stoneSprite, blib::math::easyMatrix(tiles[t.z][t.x][t.y].polygon.getCenter(), 0, 1.5f), stoneSprite->center, p->participant->color);
					spriteBatch->draw(stoneSprite, blib::math::easyMatrix(tiles[t.z][t.x][t.y].polygon.getCenter(), 0, 1.25f), stoneSprite->center, glm::vec4(0, 0, 0, 1));
				}

				spriteBatch->draw(stoneSprite, blib::math::easyMatrix(tiles[t.z][t.x][t.y].polygon.getCenter()), stoneSprite->center, p->participant->color);
			}
		}

		for (auto p : players)
			spriteBatch->draw(cursorSprite, blib::math::easyMatrix(p->cursor), cursorSprite->center, p->participant->color);

		lineBatch->end();
		spriteBatch->end();

	}

	blib::Texture * Checkers::getTitleImage()
	{
		return nullptr;
	}

}