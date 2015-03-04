#pragma once

#include "../MenuScreen.h"

class PlayAllTheGames;
class Settings;
class OptionMenuItem;

class SettingsMenu : public MenuScreen
{
	OptionMenuItem* resolution;
	OptionMenuItem* fullscreen;
	OptionMenuItem* vsync;
	OptionMenuItem* showInstructions;
public:
	SettingsMenu(Settings* settings);

	void back(PlayAllTheGames* playallthegames);
};