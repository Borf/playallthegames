#pragma once

#include <vector>
#include <blib/json.hpp>
class User;

class SessionStats
{
public:
	class Result
	{
	public:
		Result() { };
		Result(const json& config);

		std::string gameName;
		std::vector<User*> participants;
		std::vector<User*> winners;
		bool finished;
		long timeTaken;

		json asJson();
	};


	SessionStats();
	SessionStats(const json& config);
	json asJson();

	long starttime;
	std::vector<User*> users;
	std::vector<Result*> results;

};