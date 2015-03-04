#pragma once

#include <blib/util/Singleton.h>

#include <vector>
#include <string>
namespace Json { class Value; }

template<class T, class V>
class Db : public blib::util::Singleton<V>
{
private:
	std::string fileName;
protected:
public:
	std::vector<T*> data;

	void loadFromFile(std::string fileName = "");
	void saveToFile(std::string fileName = "");
	void insert(T* entry);
};


