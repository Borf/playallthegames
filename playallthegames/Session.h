#pragma once

#include <vector>
#include <glm/glm.hpp>

class Participant;
class GameSelector;
class PlayerSelector;
class PlayAllTheGames;
class GameBase;
class SessionStats;

enum class Difficulty;

class Session
{
public:
	//settings that don't change during a session
	std::vector<Participant*> participants;
	std::vector<GameBase*> games;
	GameSelector* gameSelector;
	PlayerSelector* playerSelector;

	int playerCount;
	int gameSelectorType;	//just for the menu
	int playerSelectorType; //just for the menu


	std::vector<glm::vec2> participantCursors;
	std::vector<int> participantIndex;

	Difficulty difficulty;

	SessionStats* stats;

	Session();
	void start();
	void nextGame( PlayAllTheGames* playallthegames );
	void setDebug(PlayAllTheGames* playallthegames, GameBase* game);
	void setInitialPlayers();
	void setPlayerColors();

};