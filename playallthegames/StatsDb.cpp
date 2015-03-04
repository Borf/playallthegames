#include "StatsDb.h"

#include <blib/Util.h>

StatsDb::StatsDb()
{
	loadFromFile(blib::util::getDataDir() + "/playallthegames/stats.json");
}