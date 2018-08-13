
/////////////////////////////////////////////////////////////////////////////////////
// CheckIn.cpp - Test Stub for CheckIn package                                     //
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
* CheckIn.h

* Maintenance History :
*--------------------
* ver 1.0 : 3rd March 2018

*/
#include "CheckIn.h"
#include "../FileMgr/FileMgr.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"
#include <iostream>



#ifdef TEST_CHECKIN

int main() {

	std::cout << "HELLO";
	CheckIn<PayLoad>::savingFileinCodeRepo("../Process/Process.cpp","hello.cpp.1","../root");
	std::getchar();
	return 0;
  }
#endif // TEST_CHECKIN






