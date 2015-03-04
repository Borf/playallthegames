#include "MainMenu.h"
#include "SettingsMenu.h"
#include "../PlayAllTheGames.h"
#include "../Settings.h"
#include "items/MenuItem.h"
#include "items/OptionMenuItem.h"
#include <blib/Util.h>


SettingsMenu::SettingsMenu(Settings* settings)
{
	title = "Settings";

	std::vector<std::string> resolutions;
	resolutions.push_back("1280x720");
	resolutions.push_back("1920x1079");
	resolutions.push_back("1920x1080");
	resolutions.push_back("2560x1440");

	std::vector<std::string> yesno;
	yesno.push_back("no");
	yesno.push_back("yes");

	std::string currentResolution = blib::util::toString(settings->resX) + "x" + blib::util::toString(settings->resY);
	int resIndex = -1;
	for(size_t i = 0; i < resolutions.size(); i++)
		if(resolutions[i] == currentResolution)
			resIndex = i;
	if(resIndex == -1)
	{
		resIndex = resolutions.size();
		resolutions.push_back(currentResolution);
	}

	items.push_back(resolution = new OptionMenuItem("Resolution", resolutions, resIndex));
	items.push_back(fullscreen = new OptionMenuItem("Fullscreen", yesno, settings->fullscreen ? 1 : 0));
	items.push_back(vsync = new OptionMenuItem("Vsync", yesno, settings->vsync ? 1 : 0));
	items.push_back(new OptionMenuItem("Let Borf Win", yesno));
	items.push_back(new MenuItem("Controls"));
	items.push_back(showInstructions = new OptionMenuItem("Show Instructions", yesno, settings->showInstructions ? 1 : 0));
	items.push_back(new MenuItem("Back", std::bind(&SettingsMenu::back, this, std::placeholders::_1)));
}


void SettingsMenu::back( PlayAllTheGames* playallthegames )
{
	std::vector<std::string> res = blib::util::split(resolution->options[resolution->selectedOption], "x");
	playallthegames->settings->resX = atoi(res[0].c_str());
	playallthegames->settings->resY = atoi(res[1].c_str());
	playallthegames->settings->fullscreen = fullscreen->selectedOption == 1;
	playallthegames->settings->vsync = vsync->selectedOption == 1;
	playallthegames->settings->showInstructions = showInstructions->selectedOption == 1;
	playallthegames->settings->save();
	
	playallthegames->activeMenu = new MainMenu();
}
