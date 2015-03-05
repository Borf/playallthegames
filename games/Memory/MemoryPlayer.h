#pragma once

#include "../../PlayAllTheGames/ScorePlayer.h"

#include <glm/glm.hpp>

class Settings;

class MemoryPlayer : public ScorePlayer
{
public:
	MemoryPlayer(int index);
	void setStart(Settings* settings);


	glm::vec2 cursor;
	glm::ivec2 sel;
	bool selected;


	glm::ivec2 sel2;
	bool selected2;
	float secondSelectionTime;

	bool pressedButton;

	glm::vec2 homePos;
};