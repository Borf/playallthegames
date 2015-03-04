#include "PlayerSelector.h"




FixedPlayerSelector::FixedPlayerSelector( std::vector<Participant*> participants )
{
	this->participants = participants;
}

std::vector<Participant*> FixedPlayerSelector::nextPlayers(int count)
{
	std::vector<Participant*> newPlayers;
	for (int i = 0; i < count; i++)
		newPlayers.push_back(players[i]);
	return newPlayers;
}

void FixedPlayerSelector::setInitialPlayers(const std::vector<Participant*>& players)
{
	this->players = players;
}




FairPlayerSelector::FairPlayerSelector(const std::vector<Participant*> &participants)
{
	this->participants = participants;
	for (auto p : participants)
		playCount[p] = 0;
}

void FairPlayerSelector::setInitialPlayers(const std::vector<Participant*>& players)
{
	this->players = players;
	for (auto p : players)
		playCount[p]++;
}

std::vector<Participant*> FairPlayerSelector::nextPlayers(int count)
{
	if (count == players.size())
		return players;

	int maxPlayer = 0;
	for (size_t i = 0; i < players.size(); i++)
		if (playCount[players[i]] > playCount[players[maxPlayer]])
			maxPlayer = i;

	int minParticipant = 0;
	for (size_t i = 0; i < players.size(); i++)
		if (playCount[participants[i]] > playCount[participants[maxPlayer]])
			minParticipant = i;

	players[maxPlayer] = participants[minParticipant];
	for (auto p : players)
		playCount[p]++;
	return players;
}

