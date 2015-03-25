#include "Memory.h"

#include <blib/ResourceManager.h>
#include <blib/SpriteSheet.h>
#include <blib/Texture.h>
#include <blib/SpriteBatch.h>
#include <blib/Math.h>
#include <blib/Util.h>
#include <blib/Color.h>
#include <blib/util/Profiler.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#include "Tile.h"
#include "RotatingCard.h"

std::string Memory::getName()
{
	return "Memory";
}

std::string Memory::getInstructions()
{
	return "Match the proper images together";
}

void Memory::loadResources()
{
	border = resourceManager->getResource<blib::Texture>("assets/games/Memory/border.png");
	cursor = resourceManager->getResource<blib::Texture>("assets/games/Memory/crosshair.png");
	backSprite = resourceManager->getResource<blib::Texture>("assets/games/Memory/back.png");
	selectionSprite = resourceManager->getResource<blib::Texture>("assets/games/Memory/selected.png");

	tileSheet = resourceManager->getResource<blib::SpriteSheet>("assets/games/Memory/cards1");
}

void Memory::start(Difficulty difficulty)
{
	width = 6;
	height = 6;


	totalScore = width*height/2;


	std::vector<int> randomTiles(width*height);
	int current = 0;
	std::generate(randomTiles.begin(), randomTiles.end(), [&current] () { current++; return (current-1)/2; });
	
	randomTiles = blib::math::permute(randomTiles);

	blib::linq::deleteall(rotatingCards);
	blib::linq::foreach(tiles, &blib::linq::deleteall<std::vector<Tile*> >);
	tiles.clear();
	tiles.resize(width, std::vector<Tile*>(height, NULL));

	for(size_t i = 0; i < randomTiles.size(); i++)
	{
		int x = i%width;
		int y = i/width;
		tiles[x][y] = new Tile();
		tiles[x][y]->taken = -1;
		tiles[x][y]->texture.x = randomTiles[i]%tileSheet->spriteCountX;
		tiles[x][y]->texture.y = randomTiles[i]/tileSheet->spriteCountX;
	}

	for(auto p : players) p->setStart(settings);
	gameTime = 0;
	started = false;
}

void Memory::update( float elapsedTime )
{
	gameTime += elapsedTime;
	if (gameTime > 5 && !started)
		started = true;

	int sx = settings->resY / width;
	int sy = settings->resY / height;
	int size = (sx < sy) ? (int)sx : (int)sy;

	if (started)
	{
		for (auto p : players)
		{
			p->cursor += 10.0f * p->joystick.leftStick * elapsedTime * 60.0f;

			p->cursor = glm::clamp(p->cursor, glm::vec2(0,0), settings->size);


			int x = (int)((p->cursor.x - ((settings->resX - settings->resY) / 2)) / size);
			int y = (int)(p->cursor.y / size);
			if (x >= 0 && x < width && y >= 0 && y < height && p->joystick.a == 1 && !p->pressedButton)
			{
				if (!p->selected2)
				{
					p->pressedButton = true;
					if (!p->selected)
					{
						if (!blib::linq::contains(players, [x,y] (MemoryPlayer* p2) { return p2->selected && p2->sel.x == x && p2->sel.y == y; })) //nobody else picked this tile
						{
							if (tiles[x][y]->taken == -1)
							{
								p->sel = glm::ivec2(x,y);
								p->selected = true;
								p->secondSelectionTime = gameTime;
							}
						}
					}
					else
					{
						Tile* tile = tiles[x][y];
						Tile* tile2 = tiles[p->sel.x][p->sel.y];
						if (tile->taken == -1 && tile != tile2)
						{
							if (tile->texture == tile2->texture)
							{
								for(auto p2 : players)
									if(p2->selected && p2->sel.x == x && p2->sel.y == y)
										p2->selected = false;
								tile->taken = p->index;
								tile2->taken = p->index;
								p->selected = false;

								int count = (((settings->resX - settings->resY) / 2) - size) / (size / 4);

								rotatingCards.push_back(new RotatingCard(tile->texture, glm::vec2(((settings->resX - settings->resY) / 2) + size * x + size / 2, size * y + size / 2), p->homePos + glm::vec2((p->score % count) * size / 4, size + size / 2 * (p->score / count)) ));
								rotatingCards.push_back(new RotatingCard(tile->texture, glm::vec2(((settings->resX - settings->resY) / 2) + size * p->sel.x + size / 2, size * p->sel.y + size / 2), p->homePos + glm::vec2((p->score % count) * size / 4, size + size / 2 * (p->score / count)) ));

								p->score++;
							}
							else
							{
								p->sel2 = glm::ivec2(x,y);
								p->selected2 = true;
								p->secondSelectionTime = gameTime;
							}
						}
					}

				}
			}
			else if (p->joystick.a == 0)
				p->pressedButton = false;

			if (p->selected2 && gameTime - p->secondSelectionTime > 0.75)
			{
				p->selected = false;
				p->selected2 = false;
			}

		}
	}

	for(auto r : rotatingCards)
		r->update(elapsedTime);
}

void Memory::draw()
{
	spriteBatch->begin();
	spriteBatch->draw(backSprite, blib::math::easyMatrix(backSprite, settings->screenRect));


	int sx = settings->resY / width;
	int sy = settings->resY / height;
	int size = (sx < sy) ? (int)sx : (int)sy;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			glm::vec4 c = blib::Color::white;
			MemoryPlayer* player = blib::linq::firstOrDefault<MemoryPlayer*>(players, [x,y] (MemoryPlayer* p) { return (p->sel.x == x && p->sel.y == y && p->selected) || (p->sel2.x == x && p->sel2.y == y && p->selected2); });
			if(player != NULL)
				c = player->participant->color;
			if(tiles[x][y]->taken == -1)
				spriteBatch->draw(border, blib::math::easyMatrix(border, blib::math::Rectangle(((settings->resX - settings->resY) / 2) + size * x + 4, size * y + 4, size - 8, size - 8)), c);
			if (!started || player != NULL)
			{
				blib::TextureMap::TexInfo* info = tileSheet->getFrame(tiles[x][y]->texture.x, tiles[x][y]->texture.y);

				spriteBatch->draw(info, blib::math::easyMatrix(info, blib::math::Rectangle(((settings->resX - settings->resY)/2) + size*x + 4, size*y + 4, size - 8, size - 8)));
				if(player != NULL)
					spriteBatch->draw(selectionSprite, blib::math::easyMatrix(selectionSprite, blib::math::Rectangle(((settings->resX - settings->resY) / 2) + size * x + 4, size * y + 4, size - 8, size - 8)), player->participant->color);

			}
		}
	}

	for (auto r : rotatingCards)
	{
		spriteBatch->draw(tileSheet->getFrame(r->image.x, r->image.y), blib::math::easyMatrix(r->pos, glm::degrees(r->rotation), glm::vec2((float)size / tileSheet->spriteWidth,(float)size / tileSheet->spriteWidth)), tileSheet->spriteCenter);
	}


	//foreach (MemoryPlayer p in players)
	//	spriteBatch.DrawString(Font, "Score: " + p.score, p.homePos - new Vector2(50,50), Color.White);


	for(auto p : players)
	{
		spriteBatch->draw(cursor, blib::math::easyMatrix(p->cursor, (float)(blib::util::Profiler::getAppTime()*100)), cursor->center, blib::Color::black);
		spriteBatch->draw(cursor, blib::math::easyMatrix(p->cursor, (float)(blib::util::Profiler::getAppTime()*100)), cursor->center, p->participant->color);
	}
	

	spriteBatch->end();
}

blib::Texture* Memory::getTitleImage()
{
	return NULL;
}
