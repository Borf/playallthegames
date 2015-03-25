#pragma once


#include <string>
#include <vector>
#include <list>

#include <blib/util/FileSystem.h>
#include <glm/glm.hpp>

class Player;
class Participant;
class Settings;
namespace blib { class SpriteBatch; class LineBatch; class Renderer; class ResourceManager; class Texture; 
namespace util { class FileSystemHandler;  }
};


enum class Difficulty
{
	Easy,
	Normal,
	Hard,
	Cruel,
	Wtf
};

class GameBase
{
protected:
	blib::SpriteBatch* spriteBatch;
	blib::LineBatch* lineBatch;
	blib::Renderer* renderer;
	blib::ResourceManager* resourceManager;
	Settings* settings;
public:
	float gameTime;
	glm::vec4 backgroundColor;
	enum ControllerButton
	{

	};


	GameBase();
	virtual void setAttributes(blib::SpriteBatch* spriteBatch, blib::LineBatch* lineBatch, blib::Renderer* renderer, blib::ResourceManager* resourceManager, Settings* settings);

	virtual std::string getName() = 0;
	virtual std::string getInstructions() = 0;
	virtual std::pair<int, int> getPlayerCount() = 0;

	bool resourcesLoaded;
	virtual void loadResources() = 0;
	virtual void start(Difficulty difficulty) = 0;

	
	virtual void update(float elapsedTime) = 0;
	virtual void draw() = 0;

	virtual bool hasWinner() = 0;
	virtual std::list<Player*> getWinners() = 0;

	virtual blib::Texture* getTitleImage() = 0;

	virtual std::vector<Player*> getPlayers() = 0;
	virtual void setPlayers(std::vector<Participant*>) = 0;

	/*
	virtual blib::Texture* getInstructionTexture() = 0;
	virtual std::map<ControllerButton, std::string> getKeyBindings() = 0;*/

};


template<class PlayerClass>
class Game : public GameBase
{
public:
	std::vector<PlayerClass*> players;

	std::vector<Player*> getPlayers()
	{
		return std::vector<Player*>(players.begin(), players.end());
	}

	void setPlayers(std::vector<Participant*> participants)
	{
		for(size_t i = 0; i < players.size(); i++)
			delete players[i];
		players.clear();
		for(size_t i = 0; i < participants.size(); i++)
		{
			PlayerClass* p = new PlayerClass(i);
			p->index = i;
			p->participant = participants[i];
			players.push_back(p);
		}
	}

};