#pragma once

#include <string>

#include <glm/glm.hpp>

namespace blib { namespace json { class Value; } }

class User
{
public:
	User(const blib::json::Value &v);
	User(std::string name, glm::vec4 color);
	blib::json::Value asJson() const;


	std::string name;
	glm::vec4 preferredColor;

};