#include "Game.h"
#include "Settings.h"
#include <blib/util/FileSystem.h>
#include "PlayAllTheGames.h"

std::vector<GameBase*> games;
/*void GameBase::setAttributes(blib::SpriteBatch* spriteBatch, blib::LineBatch* lineBatch, blib::Renderer* renderer, blib::ResourceManager* resourceManager, Settings* settings, const std::list<blib::util::FileSystemHandler*> &fshandlers)
{

}*/



GameBase::GameBase()
{
	resourcesLoaded = false;
	backgroundColor = glm::vec4(0.6f, 0.6f, 0.8f, 1.0f);
}

void GameBase::setAttributes(blib::SpriteBatch* spriteBatch, blib::LineBatch* lineBatch, blib::Renderer* renderer, blib::ResourceManager* resourceManager, Settings* settings)
{
	this->spriteBatch = spriteBatch;
	this->lineBatch = lineBatch;
	this->renderer = renderer;
	this->resourceManager = resourceManager;
	this->settings = settings;
}
