#include <Windows.h>
#include <blib/util/FileSystem.h>
#include <blib/Util.h>
#include "PlayAllTheGames.h"

__declspec(dllexport) int main()
{
	blib::util::fixConsole();
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(""));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler(".."));
	blib::util::FileSystem::registerHandler(new blib::util::PhysicalFileSystemHandler("../blib"));
	PlayAllTheGames* app = new PlayAllTheGames();
	app->start();
	delete app;
	blib::util::FileSystem::dispose();
	return 0;
}