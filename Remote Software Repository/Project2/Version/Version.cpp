
///////////////////////////////////////////////////////////////////////////////////
// Version.cpp - Test Stud for Version package                                   //
// ver 1.0                                                                       //
// Language:    C++, Visual Studio 2017                                          //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                 //
///////////////////////////////////////////////////////////////////////////////////


/*

* Required Files :
*-------------- -
* Payload.h
* FileSystem.h
* FileSystem.cpp
* FileMgr.h
* FileMgr.cpp
* Version.h

* Maintenance History :
*--------------------
* ver 1.0 : 3rd March 2018





*/



#include "Version.h"

#ifdef TEST_VERSION

int main() {

	DbCore<PayLoad> db;
	DbElement<PayLoad> temp;
	temp.name("TestName");
	temp.descrip("This is a test");
	db["Hello::File.cpp.1"] = temp;
	db["Hello::File.cpp.2"] = temp;
	db["Hello::File.cpp.3"] = temp;
	db["Hello::File.cpp.4"] = temp;

	DbElement<PayLoad> p;
	p.name("Prasanna");
	p.descrip("Studies At Syracuse");
	p.dateTime(DateTime().now());
	p.payLoad().value("../FileSystem/File.cpp");
	p.payLoad().categories().push_back("FileMgr");
	p.payLoad().namespc("Hello");

	int version=Version<PayLoad>::getVersion(db,p);
	std::cout << "Latest version is " << version << std::endl;
	std::getchar();
	return 0;
}
#endif // TEST_VERSION
