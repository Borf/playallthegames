#pragma once

class GameBase;

class GameSelector
{
public:
	virtual GameBase* nextGame() = 0;
};