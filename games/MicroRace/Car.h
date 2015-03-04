#pragma once

#include <glm/glm.hpp>

namespace blib { class Texture; };

class Car
{
public:
	glm::vec2 position;
	blib::Texture* sprite;
	glm::vec4 color;

	Car(glm::vec2 position, blib::Texture* sprite, glm::vec4 color);

	virtual void update(float elapsedTime, float speed);
};