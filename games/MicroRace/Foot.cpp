#include "Foot.h"

Foot::Foot( glm::vec2 position, blib::Texture* sprite, glm::vec4 color ) : Car(position, sprite, color)
{

}

void Foot::update( float elapsedTime, float speed )
{
	position = glm::vec2(position.x - 6 * (1 + (speed - 1) * 0.9f) * elapsedTime * 60, position.y);
}

