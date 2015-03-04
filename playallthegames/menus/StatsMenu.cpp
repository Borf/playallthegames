#include "MainMenu.h"
#include "StatsMenu.h"
#include "../PlayAllTheGames.h"
#include "../StatsDb.h"
#include "../User.h"

#include "items/MenuItem.h"
#include "items/OptionMenuItem.h"
#include <blib/Util.h>
#include <blib/linq.h>
#include <algorithm>


StatsMenu::StatsMenu()
{
	title = "Stats";

	items.push_back(new OptionMenuItem("Most games played", getMostGamesPlayed()));
	items.push_back(new OptionMenuItem("Most games won", getMostGamesWon()));
	items.push_back(new OptionMenuItem("Win Ratio", getWinRatio()));
	items.push_back(new MenuItem("Back", std::bind(&StatsMenu::back, this, std::placeholders::_1)));
}


void StatsMenu::back(PlayAllTheGames* playallthegames)
{
	playallthegames->activeMenu = new MainMenu();
}

std::vector<std::string> StatsMenu::getMostGamesPlayed()
{
	std::vector<std::pair<User*, int> > ret;

	std::map<User*, int> playCount;
	for (auto sessionStats : StatsDb::getInstance()->data)
		for (auto result : sessionStats->results)
			blib::linq::foreach(result->participants, [&playCount](User* user) { playCount[user]++; });

	for (auto p : playCount)
		ret.push_back(p);

	std::sort(ret.begin(), ret.end(), [](std::pair<User*, int> &a, std::pair<User*, int> &b) { return a.second > b.second;  });

	return blib::linq::select<std::vector<std::string> >(ret, [](std::pair<User*, int> p) { return p.first->name + " - " + blib::util::toString(p.second); });
}

std::vector<std::string> StatsMenu::getMostGamesWon()
{
	std::vector<std::pair<User*, int> > ret;

	std::map<User*, int> playCount;
	for (auto sessionStats : StatsDb::getInstance()->data)
		for (auto result : sessionStats->results)
			blib::linq::foreach(result->winners, [&playCount](User* user) { playCount[user]++; });

	for (auto p : playCount)
		ret.push_back(p);

	std::sort(ret.begin(), ret.end(), [](std::pair<User*, int> &a, std::pair<User*, int> &b) { return a.second > b.second;  });

	return blib::linq::select<std::vector<std::string> >(ret, [](std::pair<User*, int> p) { return p.first->name + " - " + blib::util::toString(p.second); });
}


std::vector<std::string> StatsMenu::getWinRatio()
{
	std::vector<std::pair<User*, std::pair<int, int> > > ret;

	std::map<User*, std::pair<int, int> > playCount;
	for (auto sessionStats : StatsDb::getInstance()->data)
		for (auto result : sessionStats->results)
		{
			blib::linq::foreach(result->participants, [&playCount](User* user) { playCount[user].first++; });
			blib::linq::foreach(result->winners, [&playCount](User* user) { playCount[user].second++; });
		}

	for (auto p : playCount)
		ret.push_back(p);

	std::sort(ret.begin(), ret.end(), [](std::pair<User*, std::pair<int, int> > &a, std::pair<User*, std::pair<int, int> > &b) { return a.second.second / (float) a.second.first > b.second.second / (float)b.second.first;  });

	return blib::linq::select<std::vector<std::string> >(ret, [](std::pair<User*, std::pair<int, int> > p) { return p.first->name + " - " + blib::util::toString(100 * p.second.second / (float)p.second.first); });
}
