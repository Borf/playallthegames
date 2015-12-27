#pragma once

#include <vector>
#include <string>

#include <blib/App.h>
#include <blib/KeyListener.h>
#include <blib/TextureMap.h>

namespace blib { class Texture; class FBO; class Font; struct KeyState; enum class Key; class AudioManager; class Shader; class AudioSample; }

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


	struct Audio
	{
		blib::AudioSample* gameOver;
		blib::AudioSample* tick;
		blib::AudioSample* go;
		blib::AudioSample* menu;
		std::vector<blib::AudioSample*> music;
		int activeMusic;
	} audio;


	struct ControllerMap
	{
		blib::Texture* controller;
		blib::Texture* a;
		blib::Texture* b;
		blib::Texture* x;
		blib::Texture* y;
		blib::Texture* l;
		blib::Texture* r;
		blib::Texture* dpad;
		blib::Texture* leftstick;
		blib::Texture* rightstick;
		blib::Texture* lefttrigger;
		blib::Texture* righttrigger;
	} controllerMap;



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