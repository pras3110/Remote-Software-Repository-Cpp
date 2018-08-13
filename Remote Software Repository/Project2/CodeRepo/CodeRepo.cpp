/////////////////////////////////////////////////////////////////////////////////////
// CodeRepo.cpp - Test Stub for CodeRepo package                                    //
// ver 1.0                                                                         //
// Language:    C++, Visual Studio 2017                                            //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                   //
////////////////////////////////////////////////////////////////////////////////////

/*

* Required Files :
*-------------- -
* Payload.h
* FileSystem.h
* FileSystem.cpp
* FileMgr.h
* FileMgr.cpp
* CodeRepo.h
* NoSqlDb.h

* Maintenance History :
*--------------------
* ver 1.0 : 3rd March 2018

*/





#include "CodeRepo.h"
#include "../FileMgr/FileMgr.h"
#include "../NoSQLDB/Executive/NoSqlDb.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"

using namespace FileManager;
using Dir = FileSystem::Directory;




#ifdef TEST_CODEREPO


int main() {

	using namespace NoSqlDb;
	using namespace XmlProcessing;

	DbElement<PayLoad> temp;
	temp.name("TestName");
	temp.descrip("This is a test");
	temp.payLoad().status(closed);
	CodeRepo<PayLoad> c;
	DbCore<PayLoad>& db=c.getCodeRepoDb();
	db["FileSystem::Hello.cpp.1"]=temp;
	db["FileSystem::Hello.cpp.2"] = temp;
	temp.payLoad().status(closed);
	db["FileSystem::Hello.cpp.3"] = temp;

	DbElement<PayLoad> p;
	p.name("Prasanna");
	p.descrip("Studies At Syracuse");
	p.dateTime(DateTime().now());
	p.payLoad().value("../FileSystem/Hello.cpp");
	p.payLoad().categories().push_back("FileMgr");
	p.payLoad().namespc("FileSystem");

	int finalversion=c.fileStatusVersion(3,p);
	std::cout << "Latest version is " << finalversion << std::endl;
	std::getchar();
	return 0;
}

#endif // 


