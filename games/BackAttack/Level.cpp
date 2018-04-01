#include "Level.h"
#include "BackAttack.h"
#include <blib/StaticModel.h>
#include <blib/Renderer.h>
#include <blib/RenderState.h>

#include <glm/gtc/matrix_transform.hpp>

namespace backattack
{

	Level::Models Level::models;

	Level::Level(Difficulty difficulty)
	{
		if (difficulty == Difficulty::Cruel)
			width--;
		tiles.clear(); //TODO: memory leak
		tiles.resize(width, std::vector<Tile*>(height, NULL));

		int sizex = tiles.size() - 1;
		int sizey = tiles[0].size() - 1;

		for (int i = 1; i < width; i++)
		{
			tiles[i][0] = new Tile(models.straight, 90);
			tiles[i][sizey] = new Tile(models.straight, 90);
		}

		for (int i = 1; i < height; i++)
		{
			tiles[0][i] = new Tile(models.straight, 0);
			tiles[sizex][i] = new Tile(models.straight, 0);
		}

		tiles[0][0] = new Tile(models.turn, 0);
		tiles[sizex][0] = new Tile(models.turn, -90);
		tiles[0][sizey] = new Tile(models.turn, 90);
		tiles[sizex][sizey] = new Tile(models.turn, 180);

		//sides: 0: left 1: right, 2: ???, 3: ???
		std::vector<int> data;
		if(difficulty == Difficulty::Normal)
			data.insert(data.end(), { //side, pos, offset
				2, 6, 0,
				2, 3, 0,
				1, 4, 0,
				0, 4, 0,

				1, 2, 3,
				1, 6, 3 });
		else if (difficulty == Difficulty::Wtf)
		{
			data.insert(data.end(), { //side, pos, offset
				2, 1, 0,
				2, 2, 0,
				2, 3, 0,
				2, 4, 0,
				2, 5, 0,
				2, 6, 0,
				2, 7, 0,
				2, 8, 0,

				1, 1, 0,
				1, 2, 0,
				1, 3, 0,
				1, 4, 0,
				1, 5, 0,
				1, 6, 0,
				1, 7, 0,
				1, 8, 0,
				});

			for (int i = 7; i >= 0; i--)
			{
				data.insert(data.end(), { //side, pos, offset
					0, 1, i,
					0, 2, i,
					0, 3, i,
					0, 4, i,
					0, 5, i,
					0, 6, i,
					0, 7, i,
					0, 8, i });
			}			
		}
		else if (difficulty == Difficulty::Cruel)
		{		//sides: 0: left 1: right, 2: ???, 3: ???
			data.insert(data.end(), { //side, pos, offset
					2, 4, 0,
					0, 4, 0,
					1, 4, 0
				});
		}

		for (int i = 0; i < data.size() / 3; i++)
		{
			int side = data[i*3];// i % 4;// rand() % 4;
			int pos = data[i * 3 + 1];// 2 + (rand() % (width / 2 - 2)) * 2;
			int offset = data[i * 3 + 2];// 2 + (rand() % (width / 2 - 2)) * 2;

			int dx = side >= 2 ? 0 : -(2*side-1);
			int dy = side < 2 ? 0 : -(2*(side-2)-1);

			int x = side < 2 ? (side * (width-1)) : pos;
			int y = side >= 2 ? ((side-2) * (height-1)) : pos;

			if (x >= (int)tiles.size() || y >= (int)tiles[x].size())
			{
//				i--;
				continue;
			}

			if (!tiles[x][y] || !tiles[x][y]->model || tiles[x][y]->model != models.straight)
			{
			//	i--;
				continue;
			}

			for (int i = 0; i < 30; i++)
			{
				bool doBreak = false;
				if (i >= offset)
				{
					blib::StaticModel* model = models.straight;
					float rotation = dx != 0 ? 90.0f : 0.0f;
					if (tiles[x][y] != NULL)
					{
						if (i != offset)
							doBreak = true;
						if (tiles[x][y]->model == models.junction)
							model = models.cross;
						else
							model = models.junction;
						float rots[] = { 90, -90, 0, 180 };
						rotation = rots[side];
						if (i != offset)
							rotation += 180;
					}
					tiles[x][y] = new Tile(model, rotation);
				}
				x += dx;
				y += dy;
				if (doBreak)
					break;
			}

		}


		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (tiles[x][y])
				{
					tiles[x][y]->isTrack = true;
					continue;
				}
				tiles[x][y] = new Tile(models.houses[rand() % 8], (rand() % 4) * 90.0f);
			}
		}


	}


	void Level::draw(blib::RenderState& renderState, blib::Renderer* renderer)
	{
		for (size_t x = 0; x < tiles.size(); x++)
		{
			for (size_t y = 0; y < tiles[x].size(); y++)
			{
				Tile* tile = tiles[x][y];
				if (!tile)
					continue;

				glm::mat4 mat;
				mat = glm::translate(mat, glm::vec3(8 * x, 0, 8 * y));
				mat = glm::rotate(mat, glm::radians(tile->rotation), glm::vec3(0, 1, 0));
				renderState.activeShader->setUniform(BackAttack::Uniforms::ModelMatrix, mat);
				tile->model->draw(renderState, renderer, [&renderState](const blib::Material& material)
				{
					renderState.activeTexture[0] = material.texture;
				});
			}
		}

	}


	void Level::loadResources(blib::ResourceManager* resourceManager, blib::Renderer* renderer)
	{
		models.straight = new blib::StaticModel("assets/games/BackAttack/rail_straight.fbx.json", resourceManager, renderer);
		models.turn = new blib::StaticModel("assets/games/BackAttack/rail_turn.fbx.json", resourceManager, renderer);
		models.junction = new blib::StaticModel("assets/games/BackAttack/rail_split4.fbx.json", resourceManager, renderer);
		models.cross = new blib::StaticModel("assets/games/BackAttack/rail_cross3.fbx.json", resourceManager, renderer);

		for (int i = 0; i < 8; i++)
		{
			models.houses[i] = new blib::StaticModel("assets/games/BackAttack/house_" + std::to_string(i + 1) + ".fbx.json", resourceManager, renderer);
		}

	}

}
