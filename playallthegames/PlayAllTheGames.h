#pragma once

#include <vector>
#include <string>

#include <blib/App.h>
#include <blib/KeyListener.h>

namespace blib { class Texture; class FBO; class Font; struct KeyState; enum class Key; class AudioManager; class Shader; }

class Settings;
class GameBase;
class User;

class MenuScreen;
class Session;

class PlayAllTheGames : public blib::App
{
	blib::KeyState lastKeyState;
	blib::JoyState lastJoyState;
public:
	enum State
	{
		IntroScreen,
		InMenu,
		PlayerIdentify,
		PreGame,
		InGame,
		PostGame,
		ChoosingGame,
	} activeState;
	double stateTime;

	double pregameTickTime;

	blib::AudioManager* audioManager;
	Settings* settings;
	blib::Texture* introTexture;
	blib::Texture* cursorTexture;
	blib::Font* font;


	int transin = 6;
	int transout = 5;
	std::vector<blib::Shader*> transitionShaders;
	enum class TransitionShaderUniforms
	{
		s_texture,
		transition
	};


	std::vector<GameBase*> games;

	Session* session;

	blib::FBO* gameFBO[2];
	int activeGameFbo = 0;

	GameBase* activeGame;
	MenuScreen* activeMenu;

	std::string winnerString;


	PlayAllTheGames();
	~PlayAllTheGames();

	virtual void init();

	GameBase* getGame(std::string name);

	virtual void update( double elapsedTime );
	virtual void draw();
	void switchState( State newState );
	void quit();
	void loadGames();
	bool typingMode;
};