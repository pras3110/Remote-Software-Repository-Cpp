///////////////////////////////////////////////////////////////////////////////////
// Test.cpp - Provides the Package to run test cases and define test functions   //
// ver 1.0                                                                       //
// Language:    C++, Visual Studio 2017                                          //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                 //
///////////////////////////////////////////////////////////////////////////////////


/*


   Public test functions :
    
	bool testR1()  :
	bool testR2()  :
	bool testR3()  :
	bool testR5()  :
	bool testR7()  :
	bool testR9()  :
	bool testRh()  :
	bool testR10()  :


* Required Files :
*-------------- -
* Payload.h
* FileSystem.h
* FileSystem.cpp
* FileMgr.h
* FileMgr.cpp
* NoSqlDb.h

* Maintenance History :
*--------------------
* ver 1.0 : 3rd March 2018


   This file defines 1 class :
   CodeRepoProvider :  Shares the Code Repository database across the test cases

 
 */


#include <iostream>
#include <iomanip>
#include <functional>
#include <filesystem>
#include <stdlib.h>

#include "../NoSQLDB/Executive/NoSqlDb.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"
#include "../CodeRepo/CodeRepo.h"
#include "../Process/Process.h"

using namespace Utilities;
using namespace NoSqlDb;


class CodeRepoProvider {

public:
	CodeRepo<PayLoad>& repoobj() { return c_repo; }

private:
	static CodeRepo<PayLoad> c_repo;

};

CodeRepo<PayLoad> CodeRepoProvider::c_repo;

auto putLine = [](size_t n = 1, std::ostream& out = std::cout)
{
	Utilities::putline(n, out);
};

bool testR1()
{
	Utilities::title("Demonstrating Requirement #1");
	std::cout << "\n  " << typeid(std::function<bool()>).name()
		<< ", declared in this function, "
		<< "\n  is only valid for C++11 and later versions.";
	putLine();
	return true;
}


bool testR2()
{
	Utilities::title("Demonstrating Requirement #2");
	std::cout << "\n  A visual examination of all the submitted code "
		<< "will show only\n  use of streams and operators new and delete.";
	putLine();
	return true;
}

bool testR3()
{
	Utilities::title("Demonstrating Requirement #3 : Create DBElement of type Payload");
	DbElement<PayLoad> p;
	p.name("Prasanna");
	p.descrip("sdsdfsfsf");
	p.dateTime(DateTime().now());
	p.payLoad().value("../FileSystem/FileSystem.h");
	p.payLoad().categories().push_back("FileMgr");
	p.payLoad().namespc("FileSystem::FileManager");
	p.payLoad().status(closed);
	
	CodeRepo<PayLoad> r;
	r.checkIn(p);
	p.payLoad().status(open);
	r.checkIn(p);

	DbElement<PayLoad> p1; 
	p1.payLoad().value("../Version/Version.cpp");
	p1.payLoad().categories().push_back("FileMgr");
	p1.payLoad().status(closed);
	p1.payLoad().namespc("Project2");
	r.checkIn(p1);
	p1.children().push_back("FileSystem::FileManager::FileSystem.h.2");
	r.checkIn(p1);


	// test for changing status of "FileSystem::FileManager::FileSystem.h.2" to closed from open
	DbElement<PayLoad> p2;
	p2.payLoad().value("../FileSystem/FileSystem.h");
	p2.payLoad().categories().push_back("FileMgr");
	p2.payLoad().namespc("FileSystem::FileManager");
	p2.payLoad().status(closed);
	DbCore<PayLoad>& db = r.getCodeRepoDb();
	db["FileSystem::FileManager::FileSystem.h.2"].payLoad().status(closed);
	r.closeLatestFile(p2,2);


	r.showCodeRepoDb();

	// checkout a file
	DbElement<PayLoad> checkoutdbelem;
	checkoutdbelem.payLoad().value("../LocalStorage/Version.cpp");
	checkoutdbelem.payLoad().namespc("Project2");
	
	r.checkOut(checkoutdbelem);
	putLine();
	return true;
}




bool testR5() {


	std::cout << "\n  Demonstrating code pop-up in notepad";
	std::cout << "\n ======================================";

	Process p;
	p.title("test application");
	std::string appPath = "c:/windows/system32/notepad.exe";
	p.application(appPath);

	std::string cmdLine = "/A ../root/CheckIn/CheckIn.cpp.1";
	p.commandLine(cmdLine);

	std::cout << "\n  starting process: \"" << appPath << "\"";
	std::cout << "\n  with this cmdlne: \"" << cmdLine << "\"";
	p.create();

	CBP callback = []() { std::cout << "\n  --- child process exited with this message ---"; };
	p.setCallBackProcessing(callback);
	p.registerCallback();

	std::cout << "\n  after OnExit";
	std::cout.flush();
	char ch;
	std::cin.read(&ch, 1);


	return true;
}



bool testR7() {

	CodeRepo<PayLoad> r;

	DbElement<PayLoad> temp;
	temp.name("Priya");
	temp.descrip("Works At Rapid7");
	temp.dateTime(DateTime().now());
	temp.payLoad().value("../CheckOut/CheckOut.cpp");
	temp.payLoad().categories().push_back("CheckOut");
	temp.payLoad().namespc("CheckOut");
	temp.payLoad().status(closed);

	r.checkIn(temp);


	DbElement<PayLoad> p;
	p.name("Prasanna");
	p.descrip("Studies At Syracuse");
	p.dateTime(DateTime().now());
	p.payLoad().value("../FileSystem/FileSystem.h");
	p.payLoad().categories().push_back("FileMgr");
	p.payLoad().namespc("FileSystem");
	p.payLoad().status(open);
	

	r.checkIn(p);

	DbElement<PayLoad> test5_dbElement;
	test5_dbElement.name("Sumanth");
	test5_dbElement.descrip("Works At NVIDIA");
	test5_dbElement.dateTime(DateTime().now());
	test5_dbElement.payLoad().value("../CheckIn/CheckIn.cpp");
	test5_dbElement.payLoad().categories().push_back("CheckIn");
	test5_dbElement.children().push_back("FileSystem::FileSystem.h.1");
	test5_dbElement.payLoad().namespc("CheckIn");
	test5_dbElement.payLoad().status(open);

	r.checkIn(test5_dbElement);

	r.showCodeRepoDb();

	r.writeChildToXml("../child.xml");
	r.writeParentToXml("../parent.xml");
	r.writeDatabaseToXml("../DB.xml");


	return true;
}




bool testR9() {
	Utilities::title("Demonstrating Requirement #6 : Repository Contains 3 files : 2 open checkin 1 closed check in");
	std::cout << "\n\n File Status are :\n";
	std::cout << " (0) Open \n (1) Closed \n (2) Closing" << std::endl;
	std::cout << "\n\nReading Repository database from : " <<FileSystem::Path::getFullFileSpec("../DB.xml")<< std::endl;
	CodeRepo<PayLoad> r;
	r.readChildFromXml("../child.xml");r.readParentFromXml("../parent.xml");r.ReadDatabaseFromXml("../DB.xml");
	Utilities::title("Initial Code Repository Database :\n");
	r.showCodeRepoDb();
	std::cout << "\n\nAll Files are checked in " <<FileSystem::Path::getFullFileSpec("../root")<<" Directory"<< std::endl;
	Utilities::title("Demonstrating Requirement #4 : Check In files");
	std::cout << "\n\n";
	Utilities::title("Checking in a new file in closed state , but dependent on CheckIn::CheckIn.cpp.1 :\n Changing state to closing :");
	DbElement<PayLoad> test6_dbElement;
	test6_dbElement.name("Abhijeet");test6_dbElement.descrip("Works At Zynga");
	test6_dbElement.dateTime(DateTime().now());test6_dbElement.payLoad().value("../FileMgr/FileMgr.h");
	test6_dbElement.payLoad().categories().push_back("FileMgr");test6_dbElement.payLoad().namespc("FileManager");
	test6_dbElement.children().push_back("CheckIn::CheckIn.cpp.1");test6_dbElement.payLoad().status(closed);
	r.checkIn(test6_dbElement);
	r.showCodeRepoDb();std::cout << "\n\n";
	Utilities::title("Trying to close \" CheckIn::CheckIn.cpp.1\" , but dependent on \"FileSystem::FileSystem.h.1\" :\n Changing state to closing :");
	DbElement<PayLoad> test5_dbElement;
	test5_dbElement.name("Sumanth");test5_dbElement.descrip("Works At NVIDIA");
	test5_dbElement.dateTime(DateTime().now());test5_dbElement.payLoad().value("../CheckIn/CheckIn.cpp");
	test5_dbElement.payLoad().categories().push_back("CheckIn");test5_dbElement.children().push_back("FileSystem::FileSystem.h.1");
	test5_dbElement.payLoad().namespc("CheckIn");test5_dbElement.payLoad().status(closed);
	r.checkIn(test5_dbElement);
	r.showCodeRepoDb();
	DbElement<PayLoad> p;
	p.name("Prasanna");p.descrip("Studies At Syracuse");
	p.dateTime(DateTime().now());p.payLoad().value("../FileSystem/FileSystem.h");
	p.payLoad().categories().push_back("FileMgr");p.payLoad().namespc("FileSystem");
	p.payLoad().status(closed);
	r.checkIn(p);std::cout << "\n\n";
	Utilities::title("Trying to close \" FileSystem::Filesystem.h.1\" ,not dependent on anyone :\n Changing state to closed and all other dependent closing :");
	r.showCodeRepoDb();
	DbElement<PayLoad> temp;
	temp.name("Shripad");temp.descrip("Works at Air-India");
	temp.dateTime(DateTime().now());temp.payLoad().value("../CheckOut/CheckOut.cpp");
	temp.payLoad().categories().push_back("CheckOut");temp.payLoad().namespc("CheckOut");
	temp.payLoad().status(closed);
	r.checkIn(temp);
	std::cout << "\n\n";
	Utilities::title("Checking In new File in closed state \" CheckOut::CheckOut.cpp\" :");
	r.showCodeRepoDb();
	CodeRepoProvider cp;
	cp.repoobj() = r;
	return true;
}


bool testRh() {

	Utilities::title("Demonstrating Requirement #4 : Default Checkin state is \"open\"");
	CodeRepoProvider cp;
	CodeRepo<PayLoad> r = cp.repoobj();

	
	Utilities::title("Checking in a new file \"Test.cpp\" in namespace \"Test\" default state open");

	DbElement<PayLoad> test5_dbElement;
	test5_dbElement.name("Parag");
	test5_dbElement.descrip("Worked at Adobe");
	test5_dbElement.dateTime(DateTime().now());
	test5_dbElement.payLoad().value("../Test/Test.cpp");
	test5_dbElement.payLoad().categories().push_back("CheckIn");
	
	test5_dbElement.payLoad().namespc("Test");


	
	r.checkIn(test5_dbElement);
	r.showCodeRepoDb();
	Utilities::title("Checking in a new file , where latest file is open , so updating the database");
	Utilities::title("Changing description from \" Worked at Adobe \" to \" Student at Syracuse \"");
	DbElement<PayLoad> test6_dbElement;
	test6_dbElement.name("Parag");
	test6_dbElement.descrip("Student at Syracuse");
	test6_dbElement.dateTime(DateTime().now());
	test6_dbElement.payLoad().value("../Test/Test.cpp");
	test6_dbElement.payLoad().categories().push_back("CheckIn");

	test6_dbElement.payLoad().namespc("Test");
	test6_dbElement.payLoad().status(closed);
	r.checkIn(test6_dbElement);

	r.showCodeRepoDb();

	cp.repoobj() = r;

	return true;
}

bool testR10() {

	CodeRepoProvider cp;
	CodeRepo<PayLoad> r = cp.repoobj();
	Utilities::title("Demonstrating Requirement #4 : Check Out Files");
	std::cout << "\n\n";
	Utilities::title("Checking Out File \" FileManager::FileMgr.h\" and its corresponding dependencies :");
	//Utilities::title("All CheckOut Files are stored in \"../LocalStorage\" directory :\n");
	std::cout<<"\n\nAll checked Out files are stored in "<<FileSystem::Path::getFullFileSpec("../LocalStorage/")<<" Directory" << std::endl;
	DbElement<PayLoad> checkoutdbelem;
	checkoutdbelem.payLoad().value("../LocalStorage/FileMgr.h");
	checkoutdbelem.payLoad().namespc("FileManager");
	r.showCodeRepoDb();
	r.checkOut(checkoutdbelem);
	std::cout << "\n\n";
	Utilities::title("Demonstrating Requirement #5 : Browsing");
	std::cout << "\n\n";
	Utilities::title("Browsing the Code Repository for file category = \"FileMgr\" Or \"CheckIn\":");
	std::cout << "\n\n";
	Utilities::title("Code Repository Database :");
	r.showCodeRepoDb();
	std::vector<std::string> cate_;
	cate_.push_back("FileMgr");
	cate_.push_back("CheckIn");
	std::cout << "\n\n";
	Utilities::title("After Browse Query :");
	std::cout << "\n";
	DbCore<PayLoad> db = r.BrowseByCategory(cate_);
	showDb(db);
	PayLoad::showDb(db);
	std::cout << "\n\n";
	Utilities::title("Displaying the 2nd entry of database on Notepad :");
	auto it = db.dbStore().begin();
	it++;
	std::string key = it->first;
	std::cout << "\nkey is " << key << std::endl;

	size_t indexofdot = key.find_last_of(".");
	std::string dstfilename = key.substr(0, indexofdot);
	std::string version = key.substr(indexofdot+1);

	std::stringstream stream(version);
	size_t version_int = 0;
	stream >> version_int;

	r.BrowseByFile(dstfilename,version_int);
	r.writeChildToXml("../childnext.xml");
	return true;
}



int main() {


	Utilities::Title("Testing DbCore - He said, she said database");
	putLine();

	TestExecutive ex;
	TestExecutive::TestStr ts1{ testR1, "Use C++11" };
	TestExecutive::TestStr ts2{ testR2, "Use C++11" };
	
	TestExecutive::TestStr ts5{ testR5, "Use C++11" };
	
	TestExecutive::TestStr ts7{ testR7, "Use C++11" };
	
	TestExecutive::TestStr ts9{ testR9, "Use C++11" };
	TestExecutive::TestStr ts10{ testR10, "Use C++11" };
	TestExecutive::TestStr tsrh{ testRh, "Use C++11" };

	ex.registerTest(ts1);
	ex.registerTest(ts2);
	
	//ex.registerTest(ts7);
	
	ex.registerTest(ts9);
	ex.registerTest(tsrh);
	ex.registerTest(ts10);
	

	bool result = ex.doTests();
	if (result == true)
		std::cout << "\n  all tests passed";
	else
		std::cout << "\n  at least one test failed";

	putLine(2);

	std::getchar();
	return 0;
}



