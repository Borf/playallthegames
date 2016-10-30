#pragma once

#include "../../PlayAllTheGames/AlivePlayer.h"

namespace blib { class AudioSample; }

class BulletHellPlayer : public AlivePlayer
{
public:
	BulletHellPlayer(int index);
	~BulletHellPlayer();
	glm::vec2 position;
	float rotation;
	blib::AudioSample* moveSound;

	glm::mat4 getMatrix();
};