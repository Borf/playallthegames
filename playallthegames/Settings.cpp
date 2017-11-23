#include "Settings.h"

#include <Windows.h>
#include <fstream>

#include <blib/json.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <blib/util/FileSystem.h>
#include <blib/Util.h>


Settings::Settings() : screenRect(0,0,1920,1080)
{
	json config = blib::util::FileSystem::getJson(blib::util::getDataDir() + "/playallthegames/config.json");
	if(config.is_null())
	{
		resX = 1920;
		resY = 1080;
		vsync = false;
		fullscreen = false;
		showInstructions = true;
		save();
		config = blib::util::FileSystem::getJson(blib::util::getDataDir() + "/playallthegames/config.json");
	}


	resX = config["res"][0u].get<int>();
	resY = config["res"][1u].get<int>();
	vsync = config["vsync"];
	fullscreen = config["fullscreen"];
	showInstructions = config["showinstructions"];


	setSizes();


}

void Settings::save()
{
	json config;
	config["res"].push_back(resX);
	config["res"].push_back(resY);
	config["vsync"] = vsync;
	config["fullscreen"] = fullscreen;
	config["showinstructions"] = showInstructions;

	CreateDirectory((blib::util::getDataDir() + "/playallthegames").c_str(), NULL);

	std::ofstream((blib::util::getDataDir() + "/playallthegames/config.json").c_str())<<config;
}

void Settings::setSizes()
{
	size = glm::vec2(resX, resY);
	center = size/2.0f;
	screenRect = blib::math::Rectangle(glm::vec2(0,0), size);
	scale = glm::min(resX / 1920.0f, resY / 1080.0f);
	scaleMatrix = glm::scale(glm::mat4(), glm::vec3(scale, scale,1));
}

