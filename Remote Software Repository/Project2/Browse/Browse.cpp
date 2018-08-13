/////////////////////////////////////////////////////////////////////////////////////
// Browse.cpp - Test Stub for Browse package                                       //
// ver 1.0                                                                         //
// Language:    C++, Visual Studio 2017                                           //
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
* Browse.h

* Maintenance History :
*--------------------
* ver 1.0 : 3rd March 2018

*/





#include "Browse.h"
#include <iostream>

#ifdef TEST_BROWSE



int main() {

	std::cout << "hello";
	Browse<PayLoad>::openNotepadFile("../stage/Persist.h");
	std::getchar();
	return 0;
}
#endif // TEST_BROWSE
