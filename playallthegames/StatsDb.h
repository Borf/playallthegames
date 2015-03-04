#pragma once

#include "SessionStats.h"
#include "Db.h"

class StatsDb : public Db<SessionStats, StatsDb>
{
public:
	StatsDb();

};