#include "Db.h"

#include <blib/util/FileSystem.h>
#include <blib/json.h>


template<class T, class V>
void Db<T,V>::loadFromFile( std::string fileName )
{
	if(fileName == "")
		fileName = this->fileName;
	this->fileName = fileName;
	assert(fileName != "");
	assert(data.empty());
	blib::json::Value v = blib::util::FileSystem::getJson(fileName);
	if (!v.isNull())
		for(size_t i = 0; i < v.size(); i++)
			data.push_back(new T(v[i]));
}




template<class T, class V>
void Db<T, V>::saveToFile( std::string fileName )
{
	if(fileName == "")
		fileName = this->fileName;
	this->fileName = fileName;
	assert(fileName != "");

	blib::json::Value v(blib::json::Type::arrayValue);
	for(size_t i = 0; i < data.size(); i++)
		v.push_back(data[i]->asJson());
	
	std::ofstream(fileName.c_str())<<v;
}



template<class T, class V>
void Db<T, V>::insert( T* entry )
{
	data.push_back(entry);
}






#include "UserDb.h"
template class Db<User, UserDb>;

#include "StatsDb.h"
template class Db<SessionStats, StatsDb>;