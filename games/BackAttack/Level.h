#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace blib {
	class SpriteBatch;
	class StaticModel;
	class Renderer;
	class ResourceManager;
	class RenderState;
}

enum class Difficulty;

namespace backattack
{
	class Level
	{
	public:
		int width = 10;
		int height = 9;


		class Tile
		{
		public:
			blib::StaticModel* model;
			float rotation;
			bool isTrack;

			Tile(blib::StaticModel* model, float rotation)
			{
				this->model = model;
				this->rotation = rotation;
				isTrack = false;
			}
		};

		static struct Models
		{
			blib::StaticModel* straight;
			blib::StaticModel* turn;
			blib::StaticModel* junction;
			blib::StaticModel* cross;

			blib::StaticModel* houses[10];
		} models;


		inline bool inLevel(const glm::ivec2 &pos) const
		{
			return pos.x >= 0 && pos.x < width &&
				pos.y >= 0 && pos.y < height;
		}

		std::vector<std::vector<Tile*>> tiles;

		std::vector<glm::vec2> powerups;

		Level(Difficulty difficulty);
		void draw(blib::RenderState& renderState, blib::Renderer* renderer);
		static void loadResources(blib::ResourceManager* resourceManager, blib::Renderer* renderer);
	};
}