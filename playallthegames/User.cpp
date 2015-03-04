#include "User.h"
#include <blib/json.h>


User::User( std::string name, glm::vec4 color )
{
	this->name = name;
	this->preferredColor = color;
}


User::User(const blib::json::Value &data)
{
	this->name = data["name"].asString();
	this->preferredColor.r = data["color"][0u].asInt() / 255.0f;
	this->preferredColor.g = data["color"][1u].asInt() / 255.0f;
	this->preferredColor.b = data["color"][2u].asInt() / 255.0f;
	this->preferredColor.a = 1;
}

blib::json::Value User::asJson() const
{
	blib::json::Value v;
	v["name"] = name;
	v["color"].push_back((int)(preferredColor.r * 255));
	v["color"].push_back((int)(preferredColor.g * 255));
	v["color"].push_back((int)(preferredColor.b * 255));
	return v;
}
