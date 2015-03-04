#pragma once

#include "User.h"
#include "Db.h"

class UserDb : public Db<User, UserDb>
{
public:
	UserDb();

	std::vector<User*> getRandomPlayers(int amount);

	User* getUserFromName(std::string name);
};