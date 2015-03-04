#pragma once

#include <glm/glm.hpp>

class User;

class Participant
{
public:
	User* user;
	glm::vec4 color;

	Participant(User* user);


	int totalWins;
	int totalGames;
};
