#include "SessionStats.h"
#include <blib/json.hpp>

#include "UserDb.h"
#include "User.h"

SessionStats::SessionStats()
{

}

SessionStats::SessionStats(const json& config)
{
	for (size_t i = 0; i < config["users"].size(); i++)
		users.push_back(UserDb::getInstance()->getUserFromName(config["users"][i].get<std::string>()));
	for (size_t i = 0; i < config["results"].size(); i++)
		results.push_back(new Result(config["results"][i]));
}

json SessionStats::asJson()
{
	json ret;

	ret["results"] = json::array();
	ret["users"] = json::array();

	for (size_t i = 0; i < this->users.size(); i++)
		ret["users"].push_back(this->users[i]->name);

	for (size_t i = 0; i < this->results.size(); i++)
		ret["results"].push_back(this->results[i]->asJson());

	return ret;
}


json SessionStats::Result::asJson()
{
	json ret;

	ret["name"] = gameName;
	ret["participants"] = json::array();
	ret["winners"] = json::array();
	ret["finished"] = finished;
	ret["timetaken"] = timeTaken;

	for (size_t i = 0; i < participants.size(); i++)
		ret["participants"].push_back(participants[i]->name);
	for (size_t i = 0; i < winners.size(); i++)
		ret["winners"].push_back(winners[i]->name);
	return ret;
}

SessionStats::Result::Result(const json& config)
{
	gameName = config["name"];
	finished = config["finished"];
	timeTaken = config["timetaken"].get<int>();

	for (size_t i = 0; i < config["participants"].size(); i++)
		participants.push_back(UserDb::getInstance()->getUserFromName(config["participants"][i].get<std::string>()));
	for (size_t i = 0; i < config["winners"].size(); i++)
		winners.push_back(UserDb::getInstance()->getUserFromName(config["winners"][i].get<std::string>()));
}
