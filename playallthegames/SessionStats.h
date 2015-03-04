#pragma once

#include <vector>

class User;

namespace blib { namespace json { class Value; } }

class SessionStats
{
public:
	class Result
	{
	public:
		Result() { };
		Result(const blib::json::Value& config);

		std::string gameName;
		std::vector<User*> participants;
		std::vector<User*> winners;
		bool finished;
		long timeTaken;

		blib::json::Value asJson();
	};


	SessionStats();
	SessionStats(const blib::json::Value& config);
	blib::json::Value asJson();

	long starttime;
	std::vector<User*> users;
	std::vector<Result*> results;

};