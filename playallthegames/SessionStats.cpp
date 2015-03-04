#include "SessionStats.h"
#include <blib/json.h>

#include "UserDb.h"
#include "User.h"

SessionStats::SessionStats()
{

}

SessionStats::SessionStats(const blib::json::Value& config)
{
	for (size_t i = 0; i < config["users"].size(); i++)
		users.push_back(UserDb::getInstance()->getUserFromName(config["users"][i].asString()));
	for (size_t i = 0; i < config["results"].size(); i++)
		results.push_back(new Result(config["results"][i]));
}

blib::json::Value SessionStats::asJson()
{
	blib::json::Value ret;

	ret["results"] = blib::json::Value(blib::json::Type::arrayValue);
	ret["users"] = blib::json::Value(blib::json::Type::arrayValue);

	for (size_t i = 0; i < this->users.size(); i++)
		ret["users"].push_back(this->users[i]->name);

	for (size_t i = 0; i < this->results.size(); i++)
		ret["results"].push_back(this->results[i]->asJson());

	return ret;
}


blib::json::Value SessionStats::Result::asJson()
{
	blib::json::Value ret;

	ret["name"] = gameName;
	ret["participants"] = blib::json::Value(blib::json::Type::arrayValue);
	ret["winners"] = blib::json::Value(blib::json::Type::arrayValue);
	ret["finished"] = finished;
	ret["timetaken"] = timeTaken;

	for (size_t i = 0; i < participants.size(); i++)
		ret["participants"].push_back(participants[i]->name);
	for (size_t i = 0; i < winners.size(); i++)
		ret["winners"].push_back(winners[i]->name);
	return ret;
}

SessionStats::Result::Result(const blib::json::Value& config)
{
	gameName = config["name"];
	finished = config["finished"];
	timeTaken = config["timetaken"].asInt();

	for (size_t i = 0; i < config["participants"].size(); i++)
		participants.push_back(UserDb::getInstance()->getUserFromName(config["participants"][i].asString()));
	for (size_t i = 0; i < config["winners"].size(); i++)
		winners.push_back(UserDb::getInstance()->getUserFromName(config["winners"][i].asString()));
}
