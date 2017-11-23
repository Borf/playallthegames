#include "Db.h"

#include <blib/util/FileSystem.h>
#include <blib/json.hpp>


template<class T, class V>
void Db<T,V>::loadFromFile( std::string fileName )
{
	if(fileName == "")
		fileName = this->fileName;
	this->fileName = fileName;
	assert(fileName != "");
	assert(data.empty());
	json v = blib::util::FileSystem::getJson(fileName);
	if (!v.is_null())
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

	json v = json::array();
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