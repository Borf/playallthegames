#include "GameCreateParticipantSelectMenu.h"
#include "GameCreateMenu.h"

#include "../UserDb.h"
#include "../User.h"
#include "../PlayAllTheGames.h"
#include "../Session.h"
#include "../Participant.h"
#include "items/MenuItem.h"
#include "items/ToggleMenuItem.h"

GameCreateParticipantSelectMenu::GameCreateParticipantSelectMenu( PlayAllTheGames* playallthegames )
{
	title = "Players";

	std::vector<User*> &users = UserDb::getInstance()->data;

	for(size_t i = 0; i < users.size(); i++)
	{
		bool enabled = false;
		for(size_t ii = 0; ii < playallthegames->session->participants.size() && !enabled; ii++)
			if(playallthegames->session->participants[ii]->user == users[i])
				enabled = true;

		ToggleMenuItem* item = new ToggleMenuItem(users[i]->name, enabled);
		item->color = users[i]->preferredColor;
		players.push_back(item);
		items.push_back(item);
	}

	items.push_back(new MenuItem("Back", std::bind(&GameCreateParticipantSelectMenu::back, this, std::placeholders::_1)));
}

void GameCreateParticipantSelectMenu::back( PlayAllTheGames* playallthegames )
{
	std::vector<User*> &users = UserDb::getInstance()->data;

	for(size_t i = 0; i < playallthegames->session->participants.size(); i++)
		delete playallthegames->session->participants[i];
	playallthegames->session->participants.clear();
	for(size_t i = 0; i < players.size(); i++)
	{
		if(players[i]->enabled)
			playallthegames->session->participants.push_back(new Participant(users[i])); //todo: check name or something
	}
	playallthegames->activeMenu = new GameCreateMenu(playallthegames->session);
}
