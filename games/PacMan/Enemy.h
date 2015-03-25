#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace pacman
{
	class PacManLevel;
	class PacManPlayer;

	class Enemy
	{
	public:
		glm::vec2 position;
		glm::vec2 direction;
		float offset;
		glm::vec4 color;
		int inPenStatus;


		glm::ivec2 point();

		Enemy();
		void update(PacManLevel* level, std::vector<PacManPlayer*> players, std::vector<Enemy*> enemies, float gameTime, float elapsedTime);
		virtual glm::vec2 pickDirection(PacManLevel* level, std::vector<PacManPlayer*> players, std::vector<Enemy*> enemies, float gameTime);

		std::vector<glm::vec2> getOptions(PacManLevel* level, std::vector<Enemy*> enemies, float gameTime);

	};
}