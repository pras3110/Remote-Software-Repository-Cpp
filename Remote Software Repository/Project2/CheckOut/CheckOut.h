#pragma once



/*
/////////////////////////////////////////////////////////////////////////////////////
// CheckOut.h - Provides the capability for Checking out a file from Repository     //
// ver 1.0                                                                         //
// Language:    C++, Visual Studio 2017                                           //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                  //
////////////////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- CheckOut : Template class to provide the functionality of checking out a source code file from a Code Repository

Utility functions :

1)   find_and_replace1(std::string& source, std::string const& find, std::string const& replace)
Replaces all occurences of "find" with "replace" inside the string "source"


The class functions are :

  1)  sendFilestoClient(std::string dstdir_, std::string srcfilepath_, std::string rootdir) :
         Copies a file from Repository to Local client Storage






* Required Files:
* ---------------
* FileSystem.h
* FileSystem.cpp
* FileMgr.h
* FileMgr.cpp


* Maintenance History:
* --------------------
* ver 1.0 : 3rd March 2018

*/





//#include "../NoSQLDB/Executive/NoSqlDb.h"
#include "../FileMgr/FileMgr.h"
//using namespace NoSqlDb;
using namespace FileSystem1;


void find_and_replace1(std::string& source, std::string const& find, std::string const& replace)
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
class CheckOut {


public:
	static void sendFilestoClient(std::string dstdir_, std::string srcfilepath_, std::string rootdir);



};



template<typename T>
inline void CheckOut<T>::sendFilestoClient(std::string dstdir_, std::string srcfilepath_, std::string rootdir)
{

	

	find_and_replace1(srcfilepath_, "::", "/");
	

	size_t indexoffile = srcfilepath_.find_last_of("/");
	std::string filename_ = srcfilepath_.substr(indexoffile + 1, srcfilepath_.size());
	
	size_t indexofdot = filename_.find_last_of(".");
	std::string dstfilename = filename_.substr(0, indexofdot);
	

	// here i have to form the dst path ...by extracting the filename from srcfilepath
	std::string fullsrcpath_ = rootdir + "/" + srcfilepath_;
	std::string dstfilepath_ = dstdir_ +"/"+ dstfilename;

	FileSystem1::File::copy(fullsrcpath_,dstfilepath_,false);




}
