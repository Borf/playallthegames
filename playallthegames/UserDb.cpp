#include "UserDb.h"

#include <blib/Util.h>
#include <blib/Math.h>

UserDb::UserDb()
{
	loadFromFile(blib::util::getDataDir() + "/playallthegames/users.json");
}



std::vector<User*> UserDb::getRandomPlayers(int amount)
{
	std::vector<User*> ret;

	ret.push_back(new User("Player 1", glm::vec4(1,0,0,1)));
	if(amount > 1)
		ret.push_back(new User("Player 2", glm::vec4(0,1,0,1)));
	if(amount > 2)
		ret.push_back(new User("Player 3", glm::vec4(0,0,1,1)));
	if(amount > 3)
		ret.push_back(new User("Player 4", glm::vec4(1,1,0,1)));
	if(amount > 4)
		ret.push_back(new User("Player 5", glm::vec4(1,0,1,1)));
	if(amount > 5)
		ret.push_back(new User("Player 6", glm::vec4(0,1,1,1)));
	if(amount > 6)
		ret.push_back(new User("Player 7", glm::vec4(1,1,1,1)));
	if(amount > 7)
		ret.push_back(new User("Player 8", glm::vec4(0.5,1,0.5,1)));

	for (int i = 8; i < amount; i++)
		ret.push_back(new User("Player " + blib::util::toString(i+1), glm::vec4(blib::util::hsv2rgb(glm::vec3(blib::math::randomFloat(0,1), 1.0f, 1.0f )),1)));



	return ret;
}

User* UserDb::getUserFromName(std::string name)
{
	for (size_t i = 0; i < data.size(); i++)
		if (data[i]->name == name)
			return data[i];
	return new User(name, glm::vec4(-1,-1,-1,1));
}
