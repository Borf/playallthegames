#pragma once

#include <string>
#include <blib/json.hpp>
#include <glm/glm.hpp>

class User
{
public:
	User(const json &v);
	User(std::string name, glm::vec4 color);
	json asJson() const;


	std::string name;
	glm::vec4 preferredColor;

};