#include "PlayAllTheGames.h"
#include "Game.h"
#include "User.h"
#include "Player.h"
#include "Participant.h"
#include "GameSelector.h"
#include "Session.h"
#include "DebugGameSelector.h"
#include "PlayerSelector.h"
#include "menus/MainMenu.h"
#include "menus/items/MenuItem.h"
#include "menus/items/ColorCycleMenuItem.h"
#include "Settings.h"
#include "UserDb.h"
#include "StatsDb.h"

#include <blib/Color.h>
#include <blib/Renderer.h>
#include <blib/Box2DDebug.h>
#include <blib/Util.h>
#include <blib/util/Log.h>
#include <blib/util/FileSystem.h>
#include <blib/Color.h>
#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/linq.h>
#include <blib/FBO.h>
#include <blib/Math.h>
#include <blib/Font.h>
#include <blib/audio/AudioManager.h>

#include <Windows.h>

#include <direct.h>
#include <set>
#include <glm/gtc/matrix_transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

using blib::util::Log;


#define DEBUGGAME "RushHour"

#ifdef _DEBUG
#define PREGAMETIME 0
#define POSTGAMETIME 0
#else
#define PREGAMETIME 4
#define POSTGAMETIME 4
#endif

PlayAllTheGames::PlayAllTheGames()
{
	settings = new Settings();

	appSetup.renderer = blib::AppSetup::GlRenderer;
	appSetup.title = "Play ALL the Games";
	appSetup.window.setWidth((float)settings->resX);
	appSetup.window.setHeight((float)settings->resY);

	glm::ivec2 res = blib::util::getResolution();
	if (appSetup.window.width() > res.x)
		appSetup.window.setWidth((float)res.x);
	if (appSetup.window.height() > res.y)
		appSetup.window.setHeight((float)res.y);

	//appSetup.border = true;
	appSetup.vsync = settings->vsync;	
	appSetup.joystickDriver = blib::AppSetup::DirectInput;
}


PlayAllTheGames::~PlayAllTheGames()
{
	blib::linq::deleteall(games);
	delete settings;
}

void PlayAllTheGames::init()
{
	loadGames();
	if(games.empty())
	{
		Log::out<<"No games found :(. Have you configured correctly?"<<Log::newline;
		char buf[1024];
		_getcwd(buf, 1024);
		Log::out << "Current directory: " << buf << Log::newline;
		getchar();
		exit(0);
	}
	for (auto g : games)
		g->setAttributes(spriteBatch, lineBatch, renderer, resourceManager, settings);


	session = NULL;

	UserDb::getInstance();
	StatsDb::getInstance();
	
	font = resourceManager->getResource<blib::Font>("lindsey");

	MenuScreen::loadResources(resourceManager);
	introTexture = resourceManager->getResource<blib::Texture>("assets/logo.png");
	cursorTexture = resourceManager->getResource<blib::Texture>("assets/cursor.png");
	activeGame = NULL;
	activeState = IntroScreen;
	activeMenu = NULL;
	stateTime = 0;

	//launch background thread that loads all the different apps

	gameFBO[0] = resourceManager->getResource<blib::FBO>();
	gameFBO[0]->setSize(settings->resX, settings->resY);
	gameFBO[0]->depth = true;
	gameFBO[1] = resourceManager->getResource<blib::FBO>();
	gameFBO[1]->setSize(settings->resX, settings->resY);
	gameFBO[1]->depth = true;
	lastKeyState = keyState;

	typingMode = false;

	class CharListener : public blib::KeyListener
	{
		PlayAllTheGames* patg;
	public:
		CharListener(PlayAllTheGames* patg)
		{
			this->patg = patg;
		}

		bool onChar(char character)
		{
			if (patg->typingMode)
			{
				patg->activeMenu->handleKeyboard(character);
				return true;
			}
			return false;
		}
	};

	addKeyListener(new CharListener(this));


	audioManager = blib::AudioManager::getInstance();
	audioManager->init();


	Log::out << "Loading transition shaders" << Log::newline;
	for (int i = 1; i < 9; i++)
	{
		blib::Shader* shader = resourceManager->getResource<blib::Shader>("transition" + blib::util::toString(i));
		shader->bindAttributeLocation("a_position", 0);
		shader->bindAttributeLocation("a_texture", 1);
		shader->setUniformName(TransitionShaderUniforms::s_texture, "s_texture", blib::Shader::Int);
		shader->setUniformName(TransitionShaderUniforms::transition, "transition", blib::Shader::Float);
		shader->finishUniformSetup();
		shader->setUniform(TransitionShaderUniforms::s_texture, 0);
		transitionShaders.push_back(shader);
	}


}

void PlayAllTheGames::update( double elapsedTime )
{
	stateTime += glm::min(1/60.0, elapsedTime);
	if (keyState.isPressed(blib::Key::ESC))
	{
		running = false;
		return;
	}


	static blib::Key keys[] = { blib::Key::LEFT, blib::Key::RIGHT, blib::Key::UP, blib::Key::DOWN, blib::Key::ENTER, blib::Key::Z, blib::Key::X, blib::Key::A, blib::Key::S, blib::Key::Q, blib::Key::W, blib::Key::_1, blib::Key::_2, blib::Key::D, blib::Key::F, blib::Key::G, blib::Key::R };
	static std::vector<blib::Key> keysVec(keys, keys + (sizeof(keys) / sizeof(int)));

	if(activeMenu)
		typingMode = activeMenu->canType();

	if(blib::linq::any(keysVec, [this] (blib::Key i) { return keyState.isPressed(i) || lastKeyState.isPressed(i); }))
	{
		joyStates[0].leftStick = glm::vec2(0,0);
		if (keyState.isPressed(blib::Key::LEFT))
			joyStates[0].leftStick.x--;
		if (keyState.isPressed(blib::Key::RIGHT))
			joyStates[0].leftStick.x++;
		if (keyState.isPressed(blib::Key::UP))
			joyStates[0].leftStick.y--;
		if (keyState.isPressed(blib::Key::DOWN))
			joyStates[0].leftStick.y++;

		if(!typingMode)
		{
			joyStates[0].rightStick = glm::vec2(0,0);
			if(keyState.isPressed(blib::Key::D))
				joyStates[0].rightStick.x--;
			if(keyState.isPressed(blib::Key::G))
				joyStates[0].rightStick.x++;
			if(keyState.isPressed(blib::Key::R))
				joyStates[0].rightStick.y--;
			if(keyState.isPressed(blib::Key::F))
				joyStates[0].rightStick.y++;


			joyStates[0].a = (keyState.isPressed(blib::Key::ENTER) || keyState.isPressed(blib::Key::Z)) ? 1 : 0;
			joyStates[0].b = keyState.isPressed(blib::Key::X) ? 1 : 0;
			joyStates[0].x = keyState.isPressed(blib::Key::A) ? 1 : 0;
			joyStates[0].y = keyState.isPressed(blib::Key::S) ? 1 : 0;

			joyStates[0].l = keyState.isPressed(blib::Key::Q) ? 1 : 0;
			joyStates[0].r = keyState.isPressed(blib::Key::W) ? 1 : 0;

			joyStates[0].leftTrigger = keyState.isPressed(blib::Key::_1) ? 1.0f : 0.0f;
			joyStates[0].rightTrigger = keyState.isPressed(blib::Key::_2) ? 1.0f : 0.0f;
		}

	}

	if(!typingMode)
	{
		if (keyState.isPressed(blib::Key::SPACE) && !lastKeyState.isPressed(blib::Key::SPACE))
		{
			if(session == NULL)
			{
				session = new Session();
				session->setDebug(this, getGame(DEBUGGAME));
			}
			session->nextGame(this);
			switchState(PreGame);
		}
		if (keyState.isPressed(blib::Key::DIVIDE) && !lastKeyState.isPressed(blib::Key::DIVIDE))
		{
			if (session)
			{
				session->difficulty = (Difficulty)(((int)session->difficulty + 1) % ((int)Difficulty::Wtf + 1));
				session->nextGame(this);
				switchState(PreGame);
			}
		}

	}
	
	
	if(activeState == IntroScreen)
	{
		if(stateTime > 0.1f) //check if background thread that's loading the different apps is still running too
		{
			switchState(InMenu);
			activeMenu = new MainMenu();
		}
	}
	else if(activeState == InMenu)
	{
		if(activeMenu)
		{
			activeMenu->handleInput(joyStates[0].leftStick);

			if(joyStates[0].a && !lastJoyState.a)
			{
				if(activeMenu->message != "")
					activeMenu->message = "";
				else if(activeMenu->items[activeMenu->selectedItem]->action)
					activeMenu->items[activeMenu->selectedItem]->action(this);
			}
			if (glm::length(joyStates[0].rightStick) > 0.5f)
			{
				ColorCycleMenuItem* item = dynamic_cast<ColorCycleMenuItem*>(activeMenu->items[activeMenu->selectedItem]);
				if (item)
				{
					item->hue = (float)(atan2(joyStates[0].rightStick.y, joyStates[0].rightStick.x) + blib::math::pif) / blib::math::pif * 180;
				}
			}

		}
	}

	else if(activeState == InGame)
	{
		if(activeGame)
		{
			std::vector<Player*> players = activeGame->getPlayers();
			for(size_t i = 0; i < players.size(); i++)
			{
				players[i]->prevJoystick = players[i]->joystick; //TODO: use double buffering, change joystick & prevJoystick to pointers in an array and just update the pointer, instead of copying over the struct here
				players[i]->joystick = joyStates[i];

				float xx = players[i]->joystick.leftStick.x;
				float yy = players[i]->joystick.leftStick.y;
				float len = glm::sqrt(xx*xx + yy*yy);
				float angle = atan2(yy, xx);

				if (len >= 0.25f)
					players[i]->joystick.leftStick = glm::vec2((len - 0.25f) * (1.0f / (1 - 0.25f)) * cos(angle), (len - 0.25f) * (1.0f / (1 - 0.25f)) * sin(angle));
				else
					players[i]->joystick.leftStick = glm::vec2(0, 0);


			}
		}

		if(!activeGame || activeGame->hasWinner())
		{
			std::list<Player*> winners = activeGame->getWinners();
			if(winners.empty())
				winnerString = "No winner";
			else if(winners.size() == 1)
				winnerString = "Winner: ";
			else
				winnerString = "Winners: ";
			blib::linq::foreach(winners, [this] (Player* p) { winnerString += p->participant->user->name + ", "; });
			if(!winners.empty())
				winnerString = winnerString.substr(0, winnerString.size()-2);
			blib::linq::foreach(activeGame->getPlayers(), [] (Player* p) { p->participant->totalGames++; });
			blib::linq::foreach(winners, [this] (Player* p) { p->participant->totalWins++; });
			switchState(PostGame);
			audioManager->playSound("assets/audio/sfx/gameover.wav");
		}
		else
			if (activeGame)
			{
				activeGame->gameTime += (float)elapsedTime;
				activeGame->update((float)elapsedTime);
			}
	}
	else if(activeState == PreGame)
	{
		if(pregameTickTime - (PREGAMETIME - stateTime) > 1)
		{
			if(pregameTickTime > 1)
				audioManager->playSound("assets/audio/sfx/tick.wav");
			else
				audioManager->playSound("assets/audio/sfx/go.wav");
			pregameTickTime--;
		}
		
		if(stateTime > PREGAMETIME)
		{
			switchState(InGame);
			transout = rand() % 8;
		}
	}
	else if(activeState == PostGame)
	{
		if(stateTime > POSTGAMETIME)
		{
			session->nextGame(this);
			activeGameFbo = 1 - activeGameFbo;
			transout = transin;
			transin = rand() % 8;
			switchState(PreGame);
		}

	}
	else if (activeState == PlayerIdentify)
	{
		for (size_t i = 0; i < session->participantCursors.size(); i++)
		{
			if (session->participantIndex[i] == -1)
			{

				float xx = joyStates[i].leftStick.x;
				float yy = joyStates[i].leftStick.y;
				float len = glm::sqrt(xx*xx + yy*yy);
				float angle = atan2(yy, xx);
				glm::vec2 joystick(0, 0);
				if (len >= 0.25f)
					joystick = glm::vec2((len - 0.25f) * (1.0f / (1 - 0.25f)) * cos(angle), (len - 0.25f) * (1.0f / (1 - 0.25f)) * sin(angle));

				session->participantCursors[i] += (float)elapsedTime * joystick * 1000.0f;
				session->participantCursors[i] = glm::clamp(session->participantCursors[i], glm::vec2(0, 0), glm::vec2(settings->resX, settings->resY));

				if (joyStates[i].a == 1 && (i != 0 || lastJoyState.a == 0) && session->participantCursors[i].x > 100 && session->participantCursors[i].y > 100 && session->participantCursors[i].x < settings->resX - 100 && session->participantCursors[i].y < settings->resY - 100)
				{
					int participantIndex = -1;

					int countX = (int)glm::ceil(sqrt(session->participants.size()));
					int countY = (int)glm::ceil(session->participants.size() / (float)countX);
					int w = (settings->resX - 200) / countX;
					int h = (settings->resY - 200) / countY;

					participantIndex = countX * (int)glm::floor((session->participantCursors[i].y - 100) / h) + (int)glm::floor((session->participantCursors[i].x - 100) / w);
					if (participantIndex >= 0 && participantIndex < (int)session->participants.size())
						if (!blib::linq::containsValue(session->participantIndex, participantIndex))
							session->participantIndex[i] = participantIndex;


					if (!blib::linq::containsValue(session->participantIndex, -1))
					{
						session->setInitialPlayers();
						session->nextGame(this);
						switchState(PreGame);


					}
				}

			}
		}
	}
	else
		Log::out<<"Unhandled update in state "<<activeState<<Log::newline;

	lastKeyState = keyState;
	lastJoyState = joyStates[0];
}

void PlayAllTheGames::draw()
{
	if(activeState == IntroScreen)
	{
		renderer->clear(glm::vec4(0.6f, 0.6f, 0.8f, 1.0f), blib::Renderer::Color);
		spriteBatch->begin();

		spriteBatch->draw(introTexture, blib::math::easyMatrix(glm::vec2(768,0)));

		spriteBatch->end();
	}
	else if(activeState == InMenu)
	{
		renderer->clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), blib::Renderer::Color);
		if(activeMenu)
		{
			spriteBatch->begin();
			activeMenu->draw(spriteBatch, settings);
			spriteBatch->end();
		}

	}
	else if (activeState == PlayerIdentify)
	{
		renderer->clear(blib::Color::black, blib::Renderer::Color);
		spriteBatch->begin();

		int countX = (int)glm::ceil(sqrt(session->participants.size()));
		int countY = (int)glm::ceil(session->participants.size() / (float)countX);
		int x = 100;
		int y = 100;
		int w = (settings->resX - 200) / countX;
		int h = (settings->resY - 200) / countY;

		for (size_t i = 0; i < session->participants.size(); i++)
		{
			bool taken = blib::linq::containsValue(session->participantIndex, i);
			
			spriteBatch->drawStretchyRect(MenuScreen::texture, glm::translate(glm::mat4(), glm::vec3(x, y, 0)), blib::math::Rectangle(0, 0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), glm::vec2(w, h), taken ? glm::vec4(0.3f, 0.3f, 0.3f, 1.0f) : session->participants[i]->color);
			spriteBatch->draw(font, session->participants[i]->user->name, blib::math::easyMatrix(glm::vec2(x + w / 2 - font->textlen(session->participants[i]->user->name)/2, y + h/2-12)), blib::Color::white);

			x += w;
			if (x >= settings->resX - 200)
			{
				x = 100;
				y += h;
			}
		}

		for (size_t i = 0; i < session->participants.size(); i++)
		{
			if (session->participantIndex[i] == -1)
				spriteBatch->draw(cursorTexture, blib::math::easyMatrix(session->participantCursors[i]), blib::Color::white);
		}


		spriteBatch->end();
	}
	else if(activeState == InGame || activeState == PreGame || activeState == PostGame)
	{
		spriteBatch->begin();
		if(activeState == PreGame)
		{


		}
		spriteBatch->end();
		renderer->renderState.activeFbo = gameFBO[activeGameFbo];
		spriteBatch->renderState.activeFbo = gameFBO[activeGameFbo];
		lineBatch->renderState.activeFbo = gameFBO[activeGameFbo];
		if(activeGame)
		{
			renderer->clear(activeGame->backgroundColor, blib::Renderer::Color | blib::Renderer::Depth);
			activeGame->draw();
		}
		lineBatch->renderState.activeFbo = NULL;
		spriteBatch->renderState.activeFbo = NULL;
		renderer->renderState.activeFbo = NULL;


		glm::vec4 c(1, 1, 1, 1);
		if (activeState == PreGame)
			c = glm::min(glm::vec4(stateTime, stateTime, stateTime, 1), glm::vec4(1, 1, 1, 1));
		else if (activeState == PostGame)
			c = glm::min(glm::vec4(POSTGAMETIME - stateTime, POSTGAMETIME - stateTime, POSTGAMETIME - stateTime, 1), glm::vec4(1, 1, 1, 1));

		blib::Shader* tmp = spriteBatch->renderState.activeShader;

		renderer->clear(blib::Color::black, blib::Renderer::Color);

		if (activeState == PreGame)
		{
			spriteBatch->renderState.activeShader = transitionShaders[transout];
			spriteBatch->renderState.activeShader->setUniform(TransitionShaderUniforms::transition, 0.0f);
			spriteBatch->begin();
			spriteBatch->draw(gameFBO[1 - activeGameFbo], blib::math::easyMatrix(glm::vec2(-1, -1), 0, glm::vec2(2, 2) / settings->size));
			spriteBatch->end();
		}

		spriteBatch->renderState.activeShader = transitionShaders[transin];
		spriteBatch->renderState.activeShader->setUniform(TransitionShaderUniforms::transition, c.x);
		spriteBatch->begin();
		spriteBatch->draw(gameFBO[activeGameFbo], blib::math::easyMatrix(glm::vec2(-1, -1), 0, glm::vec2(2, 2) / settings->size));
		spriteBatch->end();


		spriteBatch->renderState.activeShader = tmp;



		if(activeState == PostGame)
		{
			float x = settings->center.x - font->textlen(winnerString)/2;
			spriteBatch->begin();
			spriteBatch->drawStretchyRect(MenuScreen::texture, blib::math::easyMatrix(glm::vec2(10,50)), blib::math::Rectangle(0,0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), glm::vec2(settings->size.x-20,200), glm::vec4(1,1,1,0.75f));
			spriteBatch->draw(font, winnerString, blib::math::easyMatrix(glm::vec2(x+2,122)), blib::Color::black);
			spriteBatch->draw(font, winnerString, blib::math::easyMatrix(glm::vec2(x,120)));


			spriteBatch->drawStretchyRect(MenuScreen::texture, blib::math::easyMatrix(glm::vec2(10,250)), blib::math::Rectangle(0,0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), glm::vec2(settings->size.x-20,100+50 * session->participants.size()), glm::vec4(1,1,1,1));
			for(size_t i = 0; i < session->participants.size(); i++)
			{
				spriteBatch->draw(font, session->participants[i]->user->name, blib::math::easyMatrix(glm::vec2(100+2,300 + 50*i + 2)), blib::Color::black);
				spriteBatch->draw(font, session->participants[i]->user->name, blib::math::easyMatrix(glm::vec2(100,300 + 50*i)), session->participants[i]->color);
	
				std::string score = blib::util::toString(session->participants[i]->totalWins) + " out of " + blib::util::toString(session->participants[i]->totalGames);
				spriteBatch->draw(font, score, blib::math::easyMatrix(glm::vec2(settings->resX - 325+2,300 + 50*i + 2)), blib::Color::black);
				spriteBatch->draw(font, score, blib::math::easyMatrix(glm::vec2(settings->resX - 325,300 + 50*i)), session->participants[i]->color);
			}


			spriteBatch->end();
		}
		if(activeState == PreGame)
		{
			float alpha = (float)glm::clamp(PREGAMETIME - 0.5 - stateTime, 0.0, 1.0);
			float x = settings->center.x - font->textlen(activeGame->getInstructions())/2;
			spriteBatch->begin();
			spriteBatch->drawStretchyRect(MenuScreen::texture, blib::math::easyMatrix(glm::vec2(10,200)), blib::math::Rectangle(0,0, 100, 100), blib::math::Rectangle(32, 32, 36, 36), glm::vec2(settings->size.x-20,200), glm::vec4(1,1,1,alpha));
			spriteBatch->draw(font, activeGame->getInstructions(), blib::math::easyMatrix(glm::vec2(x+2,272)), glm::vec4(0,0,0,alpha));
			spriteBatch->draw(font, activeGame->getInstructions(), blib::math::easyMatrix(glm::vec2(x,270)), glm::vec4(1,1,1,alpha));

			float scale = (float)(2 - ((PREGAMETIME - stateTime) - (int)(PREGAMETIME - stateTime)));
			char buf[10];
			sprintf(buf, "%.1f", PREGAMETIME - stateTime);

			glm::mat4 mat;
			mat = glm::translate(mat, glm::vec3(settings->center.x - 8, 12,0));
			mat = glm::translate(mat, glm::vec3(40, 20,0));
			mat = glm::scale(mat, glm::vec3(scale, scale, 1));
			mat = glm::translate(mat, glm::vec3(-40, -20,0));
			spriteBatch->draw(font, buf, mat, blib::Color::black);

			mat = glm::translate(glm::mat4(), glm::vec3(settings->center.x - 8, 12,0));
			mat = glm::translate(mat, glm::vec3(40, 20,0));
			mat = glm::scale(mat, glm::vec3(scale - 0.1f,scale -0.1f, 1));
			mat = glm::translate(mat, glm::vec3(-40, -20,0));
			spriteBatch->draw(font, buf, mat, blib::Color::white);

			spriteBatch->end();
		}

	}

}


void PlayAllTheGames::switchState( State newState )
{
	Log::out<<"Switch to state "<<newState<<Log::newline;
	activeState = newState;
	stateTime = 0;
	if(newState == PreGame)
		pregameTickTime = PREGAMETIME;
}

void PlayAllTheGames::quit()
{
	running = false;
}

GameBase* PlayAllTheGames::getGame( std::string name )
{
	for(auto game : games)
		if(game->getName() == name)
			return game;
	return NULL;
}

