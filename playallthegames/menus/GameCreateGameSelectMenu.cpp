#include "GameCreateGameSelectMenu.h"
#include "GameCreateMenu.h"

#include "../User.h"
#include "../PlayAllTheGames.h"
#include "../Session.h"
#include "../Game.h"
#include "items/MenuItem.h"
#include "items/ToggleMenuItem.h"

GameCreateGameSelectMenu::GameCreateGameSelectMenu( PlayAllTheGames* playallthegames )
{
	title = "Games";

	for(size_t i = 0; i < playallthegames->games.size(); i++)
	{
		bool enabled = false;
		for(size_t ii = 0; ii < playallthegames->session->games.size() && !enabled; ii++)
			if(playallthegames->session->games[ii] == playallthegames->games[i])
				enabled = true;

		ToggleMenuItem* item = new ToggleMenuItem(playallthegames->games[i]->getName(), enabled);
		games.push_back(item);
		items.push_back(item);
	}

	items.push_back(new MenuItem("Back", std::bind(&GameCreateGameSelectMenu::back, this, std::placeholders::_1)));
}

void GameCreateGameSelectMenu::back( PlayAllTheGames* playallthegames )
{
	playallthegames->session->games.clear();
	for(size_t i = 0; i < games.size(); i++)
		if(games[i]->enabled)
			playallthegames->session->games.push_back(playallthegames->games[i]);
	playallthegames->activeMenu = new GameCreateMenu(playallthegames->session);
}
