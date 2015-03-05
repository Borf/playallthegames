#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"


namespace blib { class Texture; namespace math { class Rectangle; } }

class DodgeFootPlayer : public AlivePlayer
{
public:
	DodgeFootPlayer(int index);
	float position;



	blib::math::Rectangle getBounds(blib::Texture* sprite);

};