#pragma once

#include <glm/glm.hpp>

class Settings;
namespace bugswat
{
	class Enemy
	{
	public:
		glm::vec2 position;
		float rotation;
		float speed;
		bool alive;

		Enemy();


		virtual void update(float elapsedTime);

		glm::mat4 getMatrix();
		bool onScreen(Settings* settings);
	};
}