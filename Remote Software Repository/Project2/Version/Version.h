#pragma once

/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Version.h - Provides the capability for retrieving the latest version of file from Repository       //
// ver 1.0                                                                                             //
// Language:    C++, Visual Studio 2017                                                                //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                                       //
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- Browse : Template class to provide the functionality of browsing for a Code Repository
The class functions are :

    getVersion(DbCore<T>& db,DbElement<T>& dbelem) :
	    Returns the latest version of the file whose infomation is stored in DbElement elem , by
	    performing a Query on the Repository database






* Required Files:
* ---------------
* Payload.h
* NoSqlDb.h
* Maintenance History:
* --------------------
* ver 1.0 : 3rd March 2018

*/




#include "../NoSQLDB/Executive/NoSqlDb.h"
#include<iostream>
#include "../NoSQLDB/PayLoad/PayLoad.h"
using namespace NoSqlDb;
template <typename T>
class Version {

public:

	static int getVersion(DbCore<T>& db,DbElement<T>& dbelem);


};


// Returns the latest version of the file whose infomation is stored in DbElement elem , by 
 // performing a Query on the Repository database
template<typename T>
inline int Version<T>::getVersion(DbCore<T>& db,DbElement<T>& dbelem)
{
	Query<PayLoad> q(db); 
	T t = dbelem.payLoad();
	std::string namespace_ = t.namespc();
	std::string filepath_ = t.value();
	size_t indexoffile = filepath_.find_last_of("/");
	std::string filename_=filepath_.substr(indexoffile+1,filepath_.size());
	std::string key = namespace_ + "::" + filename_;
	key = key + ".*";
	q.selectkeys(key);

	if (q.keys().size() == 0) {
		return 0;
	}
	else {
		
		size_t max = 0;
		// code to return latest version
		for (auto x : q.keys()) {
			size_t indexofdot = x.find_last_of(".");
			std::string version = x.substr(indexofdot + 1, x.size());
			std::stringstream stream(version);
			size_t version_int = 0;
			stream >> version_int;
			if (max < version_int) {
				max = version_int;
			}
		}
		return max;
	}
	
	return 0;
}
