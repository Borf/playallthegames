#pragma once
/*
	* Box2D collision categories
	* 0x01 : level
	* 0x02 : player
	* 0x04 : level objects
	* 0x08 : enemies
	* 0x10 : butterflies
	* 0x20 : scarf
	*
	*
	*
	*/



#define COL_LEVEL		0x01
#define COL_PLAYER		0x02
#define COL_OBJECTS		0x04
#define COL_ENEMIES		0x08
#define COL_BUTTERFLY	0x10
#define COL_SCARF		0x20


#define COL_ALL			0xffff