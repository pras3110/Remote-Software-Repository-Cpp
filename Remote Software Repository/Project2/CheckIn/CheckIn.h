#pragma once


/*
/////////////////////////////////////////////////////////////////////////////////////
// CheckIn.h - Provides the capability for Checking in a file in the  Repository     //
// ver 1.0                                                                         //
// Language:    C++, Visual Studio 2017                                           //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                  //
////////////////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- CheckIn : Template class to provide the functionality of checking in a source code file in a Code Repository

  Utility functions :

  1)   find_and_replace(std::string& source, std::string const& find, std::string const& replace)
          Replaces all occurences of "find" with "replace" inside the string "source"

  2)   directoryExists(const std::string& path) :
         Returns the index of until which a directory exists , and how much further directory should be created
		 to copy the file

  3)   createDirectory(const std::string& exists,std::string remaining)
           Creates remaining amount of directory required to copy a particular file

The class functions are :

  1)  savingFileinCodeRepo(std::string filepath,std::string dstfilepath_,std::string rootdir) :
        copies the file from local storage to Repository storage






* Required Files:
* ---------------
* Payload.h
* FileSystem.h
* FileSystem.cpp
* FileMgr.h
* FileMgr.cpp


* Maintenance History:
* --------------------
* ver 1.0 : 3rd March 2018

*/





#include <iostream>
#include <string>
#include "../FileMgr/FileMgr.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"

using Dir = FileSystem1::Directory;
using Files = FileSystem1::File;

using namespace NoSqlDb;


void find_and_replace(std::string& source, std::string const& find, std::string const& replace)
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


size_t directoryExists(const std::string& path) {
	bool result = FileSystem1::Directory::exists(path);
	std::size_t found = path.size();

	if (!result) {
		std::size_t found = path.find_last_of("/");
		return directoryExists(path.substr(0, found));
	}
	else {
		return found;
	}

}

void createDirectory(const std::string& exists,std::string remaining) {

	std::string exists_ = exists;

	while (remaining.size() != 0) {

		size_t indexoffirst = remaining.find("/");
		std::string gotdir = remaining.substr(0,indexoffirst+1);
		exists_ = exists_ + gotdir;
		FileSystem1::Directory::create(exists_);
		remaining = remaining.substr(indexoffirst+1);

	}

}

template<typename T>
class CheckIn {


public:
	static void savingFileinCodeRepo(std::string filepath,std::string dstfilepath_,std::string rootdir);

};



// copies the file from local storage to Repository storage
template<typename T>
inline void CheckIn<T>::savingFileinCodeRepo(std::string filepath, std::string dstfilepath_, std::string rootdir)
{



	find_and_replace(dstfilepath_,"::","/");
	

	std::string fulldstpath_ = rootdir + "/" + dstfilepath_;

	// check if dst file exists....if yes it means that latest version was open...so replace the contents
	// if dst file does not exists...it means that need to create directories and then copy the file

	if (FileSystem1::File::exists(fulldstpath_)) {

		// copy from source to destination
		FileSystem1::File::copy(filepath, fulldstpath_, false);
	}
	else {
		// check if directory exists and till how much....create that many directories ...then copy
		std::size_t dirUpto = 0;
		std::size_t found = fulldstpath_.find_last_of("/");
		dirUpto = directoryExists(fulldstpath_.substr(0, found));
	
		std::string temp = fulldstpath_.substr(dirUpto + 1);
		size_t tempcount = temp.find_last_of("/");
			
		std::string remainingdir_ = temp.substr(0,tempcount+1);
	

		createDirectory(fulldstpath_.substr(0, dirUpto + 1), remainingdir_);
		
		FileSystem1::File::copy(filepath,fulldstpath_,false);


	}




}


