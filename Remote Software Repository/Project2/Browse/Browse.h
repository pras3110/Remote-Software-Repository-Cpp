#pragma once


/*
/////////////////////////////////////////////////////////////////////////////////////
// Browse.h - Provides the capability for Browsing a Repository                    //
// ver 1.0                                                                         //
// Language:    C++, Visual Studio 2017                                           //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                  //
////////////////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- Browse : Template class to provide the functionality of browsing for a Code Repository
The member functions are :

1) browseByFileName(std::string filepath,const std::string& rootdir,const std::string& stagedir) :
copies a file "filepath" from the root directory to the staging directory, which allows
'openNotepadFile' function to open that file in notepad document.

2) openNotepadFile(const std::string& path) :
creates a Process for notepad.exe to open the file "path" in a notepad application.

3) browseCategory(DbCore<T>& db,const std::string& category,std::vector<std::string>& vec) :
Runs a query on database db , and stores a set of keys in vec, which have 'category' in Payload




* Required Files:
* ---------------
* Payload.h
* CheckIn.h
* CheckOut.h
* Browse.h
* Version.h
* NoSqlDb.h
* Maintenance History:
* --------------------
* ver 1.0 : 3rd March 2018

*/




#include<iostream>
#include<vector>
#include<string>
#include<regex>
#include "../FileMgr/FileMgr.h"
#include "../Process/Process.h"
#include "../NoSQLDB/Executive/NoSqlDb.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"

using namespace FileSystem1;
using namespace NoSqlDb;


// Utility function to replace all occurences of "::" with "/"
void find_and_replace2(std::string& source, std::string const& find, std::string const& replace)
{
	size_t i = 0;
	i = source.find(find, i);
	for (size_t x = i; x != std::string::npos;)
	{
		source.replace(x, find.length(), replace);
		x += replace.length();
		x = source.find(find, x);

	}
}


template <typename T>
class Browse {

public:

	static void browseCategory(DbCore<T>& db, const std::string& category, std::vector<std::string>& vec);
	static void browseByFileName(const DbCore<T>& db,const std::string & filename, std::vector<std::string> & vec);
	static void browseByVersion(const DbCore<T>& db, const std::string & version, std::vector<std::string> & vec);
	static void browseByChildren(const DbCore<T>& db, const std::string & children, std::vector<std::string> & vec);
	static void openNotepadFile(const std::string& path);


};



//copies a file "filepath" from the root directory to the staging directory, which allows
//'openNotepadFile' function to open that file in notepad document.

template<typename T>
inline void Browse<T>::browseByFileName(const DbCore<T>& db,const std::string & filename, std::vector<std::string> & vec)
{
	DbCore<T> temp = db;
	vec.clear();
	std::string exp = "(.*)(" + filename + ")(.*)";
	std::regex temp_regex(exp);
	for (auto x : temp)
	{
		if (std::regex_match(x.first, temp_regex))
			vec.push_back(x.first);
	}
	//Querying DB on basis of query.
}

template<typename T>
inline void Browse<T>::browseByVersion(const DbCore<T>& db, const std::string & version, std::vector<std::string> & vec) {
	
	DbCore<T> temp = db;
	
	vec.clear();
	for (auto x : temp)
	{
		size_t indexofVersion = x.first.find_last_of(".");
		std::string key_version = x.first.substr(indexofVersion + 1, x.first.size());
		if (key_version == version)
			vec.push_back(x.first);
	}



}

template<typename T>
inline void Browse<T>::browseByChildren(const DbCore<T>& db, const std::string & children, std::vector<std::string> & vec) {

	DbCore<T> fulldb = db;
	vec.clear();
	Query<T> q1(fulldb);
	Keys keys{ children };
	Conditions<PayLoad> conds0;
	conds0.children(keys);
	vec = q1.select(conds0).keys();

}


// creates a Process for notepad.exe to open the file "path" in a notepad application.
template<typename T>
inline void Browse<T>::openNotepadFile(const std::string & path)
{
	Process p;
	p.title("test application");
	std::string appPath = "c:/windows/system32/notepad.exe";
	p.application(appPath);

	std::string cmdLine = "/A " + path;
	p.commandLine(cmdLine);

	std::cout << "\n  starting process: \"" << appPath << "\"";
	std::cout << "\n  with this cmdlne: \"" << cmdLine << "\"";
	p.create();
	
}



//  Runs a query on database db , and stores a set of keys in vec, which have 'category' in Payload
template<typename T>
inline void Browse<T>::browseCategory(DbCore<T>& db, const std::string & category, std::vector<std::string>& vec)
{

	Query<T> q1(db);
	auto hasCategory = [&category](DbElement<PayLoad>& elem) {
		return (elem.payLoad()).hasCategory(category);
	};
	vec = q1.select(hasCategory).keys();

}
