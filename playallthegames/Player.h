#pragma once

class Participant;

#include <blib/App.h>
#include <blib/JoyState.h>

class Player
{
public:
	Player(int index) { this->index = index; };
	int index;
	Participant* participant;
	blib::JoyState joystick;
	blib::JoyState prevJoystick;
};