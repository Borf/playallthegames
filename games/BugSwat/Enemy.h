#pragma once

#include <glm/glm.hpp>

namespace blib { class AudioSample;  }
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
		blib::AudioSample* flySound;


		Enemy();
		~Enemy();


		virtual void update(float elapsedTime);

		glm::mat4 getMatrix();
		bool onScreen(Settings* settings);
	};
}