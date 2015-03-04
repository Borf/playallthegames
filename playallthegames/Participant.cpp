#include "Participant.h"

#include "User.h"

Participant::Participant( User* user )
{
	this->user = user;
	this->color = user->preferredColor;
	totalWins = 0;
	totalGames = 0;
}


