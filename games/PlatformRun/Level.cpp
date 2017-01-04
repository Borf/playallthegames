#include "Level.h"

#include <blib/ResourceManager.h>
#include <blib/Texture.h>
#include <blib/SpriteBatch.h>
#include <blib/Math.h>
#include <blib/util/Log.h>

using blib::util::Log;

#include <glm/gtc/noise.hpp>

namespace platformrun
{
	Level::Level(blib::ResourceManager* resourceManager)
	{
		texturemap = resourceManager->getResource<blib::Texture>("assets/games/PlatformRun/level.png");
		
		levelData = new int[17 * 128];
		memset(levelData, 0, sizeof(levelData));


		buildSection(0,0);
		buildSection(64,1);


	}

	static int tileset2[] = { 44, 45, 46, 48 , 82, 83 };

	void Level::buildSection(int offset, int screen)
	{
		Log::out << "Building section " << screen << Log::newline;

		memset(levelData + offset * 17, 0, 64 * 17 * sizeof(int));

		int tileset = rand() % 2;

		int noiseHeight = 4;
		float noiseFreq = blib::math::randomFloat(5, 7);
		if (screen > 1)
		{
			noiseHeight = 6;
			noiseFreq = blib::math::randomFloat(8,10);
		}
		if (screen > 3)
			noiseHeight = 10;
		if(screen > 4)
			noiseFreq = blib::math::randomFloat(15, 17);

		std::vector<int> heights;
		for (int i = offset; i < offset+64; i++)
		{
			int h = (int)(2 + noiseHeight * abs(glm::noise1(i / noiseFreq)));
			heights.push_back(h);
			buildHeight(i, h, tileset);
		}

		int gapCount = 1 + rand() % 2;
		if(screen > 2)
			gapCount = 2 + rand() % 5;
		if (screen > 4)
			gapCount = 4 + rand() % 5;

		for (int i = 0; i < gapCount; i++)
		{
			int width = 1 + rand() % 3;
			if(screen > 4)
				int width = 1 + rand() % 6;

			int x = offset + rand() % (64 - width);
			if (screen == 0 && x < 30)
				continue;


			for (int xx = x; xx < x + width; xx++)
			{
				heights[xx - x] = 0;
				for (int y = 0; y < 17; y++)
					l(xx, y) = 0;
			}
		}



		int flowerCount = 5+rand() % 10;
		for (int i = 0; i < flowerCount; i++)
		{
			int x = offset + rand() % 64;
			for (int i = 0; i < 17; i++)
			{
				if (l(x, i) == 42)
				{
					l(x, i - 1) = 15;
					break;
				}
			}
		}

		int faceCount = 1 + rand() % 10;
		for (int i = 0; i < faceCount; i++)
		{
			int x = rand() % 63;
			if (heights[x] > 2 && heights[x + 1] > 2)
			{
				int h = glm::min(heights[x], heights[x + 1]) - 1;

				if (l(offset + x, 17 - h) != 5 ||
					l(offset + x+1, 17 - h) != 5 ||
					l(offset + x, 17 - h+1) != 5 ||
					l(offset + x+1, 17 - h+1) != 5)
					continue;

				l(offset + x, 17-h) = 49;
				l(offset + x + 1, 17-h) = 50;
				l(offset + x, 17-h+1) = 51;
				l(offset + x + 1, 17-h+1) = 52;
			}
		}


		if (tileset == 1)
		{
			for (int x = offset+1; x < offset + 63; x++)
			{
				int h = heights[x - offset];
				if (h == 0)
					continue;
				if (l(x, 17 - h) == 0)
					continue;
				if (heights[x - offset - 1] != h && heights[x - offset + 1] != h)
					l(x, 17 - h) = tileset2[rand() % 6];
				if (heights[x - offset - 1] == h && heights[x - offset + 1] != h)
					l(x, 17-h)++;
				if (heights[x - offset - 1] != h && heights[x - offset + 1] == h)
					l(x, 17-h)--;
			}
		}
	}

	void Level::buildHeight(int x, int height, int tileset)
	{
		assert(height > 0);

		int vine = rand() % 1000 < 50 ? 1 : 0;


		for (int i = 0; i < height; i++)
			if(tileset == 0)
				l(x, 16 - i) = (i == height - 1) ? 42-vine : 5-vine;
			else if(tileset == 1)
				l(x, 16 - i) = (i == height - 1) ? 70 : tileset2[rand()%6];

	}



	void Level::draw(blib::SpriteBatch & spriteBatch, float offset)
	{
		int screens = offset / (64 * 64);
		offset = offset - screens * 64 * 64;
		if (lastOffset != screens)
		{
			memcpy(levelData, levelData + 17 * 64, 17 * 64 * sizeof(int));
			buildSection(64, screens);
		}
		lastOffset = screens;

		for (int x = (offset/64)-2; x < (offset / 64) + 32; x++)
		{
			for (int y = 0; y < 17; y++)
			{
				if (x < 0)
					continue;
				int index = l(x,y);
				if (index == 0)
					continue;
				blib::math::Rectangle rect(glm::vec2(1.0f / 19.0f * (index%19), 1.0f / 6.0f * (index/19)), 1.0f / 19.0f, 1.0f / 6.0f);
				spriteBatch.draw(texturemap, blib::math::easyMatrix(glm::vec2(64*x - offset, 64*y)), glm::vec2(0,0), rect);
			}
		}

	}
	int & Level::l(int x, int y)
	{
		static int oob = 0;

		if (!(x >= 0 && x < 128 && y >= 0 && y < 17))
			return oob;
		//return levelData[x + 64 * y];
		return levelData[x * 17 + y];
	}

	bool Level::blocking(int x, int y)
	{
		int tile = l(x, y);
		if (tile == 0)
			return false;
		if (tile == 15)
			return false;
		return true;



	}
}