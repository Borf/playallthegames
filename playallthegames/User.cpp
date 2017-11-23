#include "User.h"
#include <blib/json.hpp>


User::User( std::string name, glm::vec4 color )
{
	this->name = name;
	this->preferredColor = color;
}


User::User(const json &data)
{
	this->name = data["name"].get<std::string>();
	this->preferredColor.r = data["color"][0u].get<int>() / 255.0f;
	this->preferredColor.g = data["color"][1u].get<int>() / 255.0f;
	this->preferredColor.b = data["color"][2u].get<int>() / 255.0f;
	this->preferredColor.a = 1;
}

json User::asJson() const
{
	json v;
	v["name"] = name;
	v["color"].push_back((int)(preferredColor.r * 255));
	v["color"].push_back((int)(preferredColor.g * 255));
	v["color"].push_back((int)(preferredColor.b * 255));
	return v;
}
