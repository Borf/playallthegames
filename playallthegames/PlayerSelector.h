#pragma once

#include <vector>
#include <map>

class Participant;

class PlayerSelector
{
public:
	virtual std::vector<Participant*> nextPlayers(int count) = 0;
	virtual void setInitialPlayers(const std::vector<Participant*>& players) = 0;
};


class FixedPlayerSelector : public PlayerSelector
{
	std::vector<Participant*> participants;
	std::vector<Participant*> players;
public:
	FixedPlayerSelector(std::vector<Participant*> participants);
	virtual std::vector<Participant*> nextPlayers(int count);
	virtual void setInitialPlayers(const std::vector<Participant*>& players);

};


class FairPlayerSelector : public PlayerSelector
{
	std::vector<Participant*> participants;
	std::vector<Participant*> players;
	std::map<Participant*, int> playCount;
public:
	FairPlayerSelector(const std::vector<Participant*> &participants);
	virtual std::vector<Participant*> nextPlayers(int count);
	virtual void setInitialPlayers(const std::vector<Participant*>& players);
};