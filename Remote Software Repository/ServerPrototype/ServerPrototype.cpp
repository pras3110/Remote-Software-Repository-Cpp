/////////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages      //
// ver 1.1                                                                 //
// Language:    C++, Visual Studio 2017                                    //
// Author - Prasanna Dodwad ,CSE687 - Object Oriented Design, Spring 2018  //
// Source - Jim Fawcett, Syracuse University, CST 4-187                    //
/////////////////////////////////////////////////////////////////////////////


/*


Package Operations:
* -------------------
* This package provides 1 class 'Server' :

   The public functions are :

   - Server::getFiles(const Repository::SearchPath& path) : Returns the files in a specified path
   -Server::getDirs(const Repository::SearchPath& path) : Returns the directories in specified path
   - void show(const T& t, const std::string& msg) : Displays message on the console

   Dispatcher functors :

   std::function<Msg(Msg)> echo - Executed when echo command is found in message

   std::function<Msg(Msg)> connectFromClient - Executed on connect request from client

   std::function<Msg(Msg)> sendToRepo - Executed when server receives file from client

   std::function<Msg(Msg)> sendFileToClient - Executed when server sends file to client

   std::function<Msg(Msg)> sendFileToClientMetadata - Executed to send metadata to client

   std::function<Msg(Msg)> getFiles_browse - Executed to get files of server for browse tab

   std::function<Msg(Msg)> getDirs_browse  - Executed to get directories of server for browse tab

   std::function<Msg(Msg)> getDirs_metadata  - Executed to get directories of server for metadata tab

   std::function<Msg(Msg)> getFiles_metadata  - Executed to get files of server for metadata tab

   std::function<Msg(Msg)> getDirs_checkout  - Executed to get directories of server for checkout tab

   std::function<Msg(Msg)> getFiles_checkout  - Executed to get files of server for checkout tab

   std::function<Msg(Msg)> getDirs_checkin  - Executed to get directories of server for checkin tab

   std::function<Msg(Msg)> getDirs_file  - Executed to get directories of server for file tab

   std::function<Msg(Msg)> getFiles_file  - Executed to get files of server for file tab

   std::function<Msg(Msg)> checkout - Executed when checkout command is received from client

   std::function<Msg(Msg)> viewFile - Executed when viewfile command is received from client

   std::function<Msg(Msg)> fileReceivedAck - Executed when client sends 'sentfile' command

   std::function<Msg(Msg)> multiBrowse - Executed for Multibrowse facility



Required Files:
* ---------------
* ServerPrototype.h , FileSystem.h

* Maintenance History:
* --------------------
  ver 1.2 : May 1st , 2018
 
* ver 1.1 : April 10th, 2018
- More serverProc dispatchers are added to handle Server Repository functions

* ver 1.0 : March 27th , 2018
*   - first release


*/




#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include "../Project2/ExecutiveCodeRepo/ExecutiveCodeRepo.h"
#include <chrono>
#include <string>
#include <vector>

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem1;
using Msg = MsgPassingCommunication::Message;


 // Constructs vector of childrens from a string
std::vector<std::string> findChildrens(const std::string& children) {

	size_t index;
	size_t start = 0;
	std::vector<std::string> result;
	index = children.find(";");
   
	while (index != std::string::npos) {
		std::string temp = children.substr(start,index-start);
		result.push_back(temp);
		start = index + 1;
		index = children.find(";", start);

	}


	return result;

}

 // Breaks the string based on ';' into vector of strings
std::vector<std::string> browseDelimiter(const std::string& children) {

	size_t index;
	size_t start = 0;
	std::vector<std::string> result;
	index = children.find(";");

	while (index != std::string::npos) {
		std::string temp = children.substr(start, index - start);
		result.push_back(temp);
		start = index + 1;
		index = children.find(";", start);

	}
	std::string temp = children.substr(start);
	result.push_back(temp);
	return result;

}


  // converts checkout file extension by removing .
std::vector<std::string> checkoutFiles(std::vector<std::string> checkoutFilesWithDot) {

	std::vector<std::string> result;
	for (auto x : checkoutFilesWithDot) {
		size_t indexofdot=x.find_last_of(".");
		std::string temp = x.substr(0, indexofdot);
		size_t indexofdotdot = temp.find("::");
		std::string temp2 = temp.substr(indexofdotdot+2);
		result.push_back(temp2);

	}
	return result;

}


// Shares the Repository Database among the Server Procs
class ServerProvider {

public:
	Server & serverobj() { return serverob; }

private:
	static Server serverob;

};

Server ServerProvider::serverob;




//  Returns the files in a specified path
Repository::Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}
// Returns the directories in specified path
Dirs Server::getDirs(const Repository::SearchPath& path)
{
  return Directory::getDirectories(path);
}


// Displays message on the console
template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

//  Executed when echo command is found in message
std::function<Msg(Msg)> echo = [](Msg msg) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};


//  Executed on connect request from client

std::function<Msg(Msg)> connectFromClient = [](Msg msg) {

	ServerProvider s;
	Server s1;
	s1 = s.serverobj();
	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("readytoconnect");
	reply.attribute("content","Hello from Server...!!");
	return reply;

};

// Executed when server receives file from client
std::function<Msg(Msg)> sendToRepo = [](Msg msg) {

	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("filexyz",msg.value("file"));
	

	reply.attribute("source","repository");
	reply.attribute("destination","client");
	
	reply.attribute("content","Repo receiving file from Client..");
	reply.command("gotfile");
	//reply.remove("file");
	return reply;
};

// Executed when server sends file to client
std::function<Msg(Msg)> sendFileToClient = [](Msg msg) {

	// first copy file to send files for server
	std::string filename = msg.value("filename");

	size_t indexofdotdot = filename.find("::");
	std::string npspc = filename.substr(0,indexofdotdot);
	std::string fnamewithversion = filename.substr(indexofdotdot+2);
	std::string fnamewithoutversion = fnamewithversion.substr(0,fnamewithversion.find_last_of("."));
	FileSystem1::File::copy("../Storage/"+npspc+"/"+fnamewithversion,"../SendFilesRepo/"+fnamewithoutversion,false);

	
	

	Msg reply=msg;
	std::cout << reply.toString() << std::endl;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("file",fnamewithoutversion);
	reply.attribute("filename",fnamewithoutversion);
	reply.attribute("command","sendingfiletobrowse");
	reply.attribute("content","Sending file to client..");
	//reply.attribute("path",path);
	reply.attribute("source","repository");
	reply.attribute("destination","client");

	return reply;
};



// Executed to send metadata to client
std::function<Msg(Msg)> sendFileToClientMetadata = [](Msg msg) {

	std::string filename = msg.value("filename");
	std::string path = msg.value("path");
	//FileSystem::File::copy(path + "/" + filename, sendFilePathForServer + "/" + filename, false);

	Msg reply = msg;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.attribute("author","Prasanna");
	reply.attribute("date","10/31/1995");
	reply.attribute("language","C++ and C#");
	reply.command("sentmetadata");
	reply.attribute("content","Sent the metadata");
	return reply;
};


// Executed to get files of server for browse tab
std::function<Msg(Msg)> getFiles_browse = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFilesbrowse");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Repository::Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};

// Executed to get directories of server for browse tab
std::function<Msg(Msg)> getDirs_browse = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirsbrowse");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Repository::Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};




// Executed to get dirs of server for metadata tab
std::function<Msg(Msg)> getDirs_metadata = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getDirsmetadata");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Repository::Files dirs = Server::getDirs(searchPath);
		size_t count = 0;
		for (auto item : dirs)
		{
			if (item != ".." && item != ".")
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("dir" + countStr, item);
			}
		}
	}
	else
	{
		std::cout << "\n  getDirs message did not define a path attribute";
	}
	return reply;
};

// Executed to get files of server for metadata tab
std::function<Msg(Msg)> getFiles_metadata = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFilesmetadata");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Repository::Files files = Server::getFiles(searchPath);
		size_t count = 0;
		for (auto item : files)
		{
			std::string countStr = Utilities::Converter<size_t>::toString(++count);
			reply.attribute("file" + countStr, item);
		}
	}
	else
	{
		std::cout << "\n  getFiles message did not define a path attribute";
	}
	return reply;
};

 // Executed to get directories of server for checkout tab
std::function<Msg(Msg)> getDirs_checkout = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getDirscheckout");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Repository::Files dirs = Server::getDirs(searchPath);
		size_t count = 0;
		for (auto item : dirs)
		{
			if (item != ".." && item != ".")
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("dir" + countStr, item);
			}
		}
	}
	else
	{
		std::cout << "\n  getDirs message did not define a path attribute";
	}
	return reply;
};


// Executed to get files of server for checkout tab
std::function<Msg(Msg)> getFiles_checkout = [](Msg msg) {
	ServerProvider s;
	Server server;
	server = s.serverobj();
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFilescheckout");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Repository::Files files = Server::getFiles(searchPath);
		size_t count = 0;
		for (auto item : files)
		{
			std::string countStr = Utilities::Converter<size_t>::toString(++count);
			reply.attribute("file" + countStr, item);
			size_t indexoffile = path.find_last_of("/");
			std::string nmspc_ = path.substr(indexoffile + 1, path.size());
			DbElement<PayLoad> dbelem = server.getRepo().getCodeRepoDb()[nmspc_ + "::" + item];
			reply.attribute("author" + countStr, dbelem.name());
			reply.attribute("description" + countStr, dbelem.descrip());
			std::string temp_status;

			if (dbelem.payLoad().status() == open) {
				temp_status = "Open";
			}
			else if (dbelem.payLoad().status() == closing) {
				temp_status = "Closing";
			}
			else if (dbelem.payLoad().status() == closed) {
				temp_status = "Closed";
			}
			reply.attribute("status" + countStr, temp_status);
			//std::cout << "hahahah";
		}
	}
	else
	{
		std::cout << "\n  getFiles message did not define a path attribute";
	}
	return reply;
};


// Executed to get directories of server for checkin tab
std::function<Msg(Msg)> getDirs_checkin = [](Msg msg) {

	
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getDirscheckin");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Repository::Files dirs = Server::getDirs(searchPath);
		size_t count = 0;
		for (auto item : dirs)
		{
			if (item != ".." && item != ".")
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("dir" + countStr, item);
			}
		}
	}
	else
	{
		std::cout << "\n  getDirs message did not define a path attribute";
	}
	return reply;

	
	
};


// Executed to get directories of server for file tab
std::function<Msg(Msg)> getDirs_file = [](Msg msg) {


	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getDirsfile");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Repository::Files dirs = Server::getDirs(searchPath);
		size_t count = 0;
		for (auto item : dirs)
		{
			if (item != ".." && item != ".")
			{
				std::string countStr = Utilities::Converter<size_t>::toString(++count);
				reply.attribute("dir" + countStr, item);
			}
		}
	}
	else
	{
		std::cout << "\n  getDirs message did not define a path attribute";
	}
	return reply;



};

// Executed to get files of server for file tab
std::function<Msg(Msg)> getFiles_file = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFilesfile");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Repository::Files files = Server::getFiles(searchPath);
		size_t count = 0;
		for (auto item : files)
		{
			std::string countStr = Utilities::Converter<size_t>::toString(++count);
			reply.attribute("file" + countStr, item);
		}
	}
	else
	{
		std::cout << "\n  getFiles message did not define a path attribute";
	}
	return reply;
};



// Executed when checkout command is received from client
std::function<Msg(Msg)> checkout = [](Msg msg) {

	ServerProvider s;
	Server server;
	server = s.serverobj();
	std::string totalfiles;
	DbElement<PayLoad> dbelem;
	//dbelem.name(msg.value("author"));
	//dbelem.descrip(msg.value("description"));
	//dbelem.dateTime(DateTime().now());
	dbelem.payLoad().value("../ServerRepository/" + msg.value("filename"));
	dbelem.payLoad().namespc(msg.value("namespace"));
	server.getRepo().checkOut(dbelem);

	// transfer the files from Repository to Client
	std::string key = msg.value("namespace") + "::" + msg.value("filename");
	std::vector<std::string> files = server.getRepo().listOfDependentFiles(key);
	std::vector<std::string> fileswithoutdot = checkoutFiles(files);

	for (auto x : fileswithoutdot) {

		totalfiles = totalfiles + x + ";";
	}

	
		
		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		//reply.attribute("file", x);
		//reply.attribute("filename", x);
		reply.attribute("totalfiles",totalfiles);
		reply.attribute("command", "checkoutfile");
		reply.attribute("content", "Checking out file to client..");
		//reply.attribute("path", path);
		reply.attribute("source", "repository");
		reply.attribute("destination", "client");
		reply.attribute("tab","checkout");

		
		//reply.command("checkoutdone");
		//reply.attribute("content", "File Checked out...!");
		return reply;

	

	

};


// Executed when viewfile command is received from client
std::function<Msg(Msg)> viewFile = [](Msg msg) {

	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("filegiven");
	reply.attribute("content", "Sent file to Client...!");

	return reply;

};

// Executed when client sends 'sentfile' command
std::function<Msg(Msg)> fileReceivedAck = [](Msg msg) {
	ServerProvider s;
	Server server;
	server = s.serverobj();
	std::vector<std::string> childs;
	if (msg.containsKey("children")) {
		 childs = findChildrens(msg.value("children"));
	}
	std::vector<std::string> cates = browseDelimiter(msg.value("category"));
	std::string value = msg.value("value");
	find_and_replace(value,"\\","/");
	DbElement<PayLoad> dbelem;
	dbelem.name(msg.value("author"));
	dbelem.descrip(msg.value("description"));
	dbelem.dateTime(DateTime().now());
	dbelem.payLoad().value("../ServerRepository/" + msg.value("filename"));
	dbelem.payLoad().namespc(msg.value("namespace"));
	dbelem.payLoad().categories() = cates;
	std::string latestauthor = server.getRepo().getAuthor(dbelem);
	if ((latestauthor != msg.value("author")) && (latestauthor != "empty")) {

		Msg reply;
		reply.to(msg.from());
		reply.from(msg.to());
		reply.command("singleownershipfail");
		return reply;

	}
	if (msg.containsKey("children")) {
		dbelem.children() = childs;
	}
	if (msg.value("status") == "Open") {
		dbelem.payLoad().status(open);
	}
	else if (msg.value("status") == "Closed") {
		dbelem.payLoad().status(closed);
	}
	server.getRepo().checkIn(dbelem);
	server.getRepo().showCodeRepoDb();
	//server.persistCodeRepo();
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("filereceivedack");
	reply.attribute("content", "File received by Repo...!");
	s.serverobj() = server;
	return reply;
};

// Creates a Folder in Repository structure
std::function<Msg(Msg)> createFolder = [](Msg msg) {


	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("createdfolder");
	
	FileSystem1::Directory::create(storageRoot + "/" + msg.value("fname"));
	reply.attribute("content", "Created Folder..!");

	return reply;

};

// gets DB list for checkin
std::function<Msg(Msg)> getFiles_checkin = [](Msg msg) {

	ServerProvider s;
	Server server;
	server = s.serverobj();
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFilescheckin");
	std::string path = msg.value("path");
	if (path != "")
	{
		std::string searchPath = storageRoot;
		if (path != ".")
			searchPath = searchPath + "\\" + path;
		Repository::Files files = Server::getFiles(searchPath);
		size_t count = 0;
		for (auto item : files)
		{
			std::string countStr = Utilities::Converter<size_t>::toString(++count);
			reply.attribute("file" + countStr, item);
			size_t indexoffile = path.find_last_of("/");
			std::string nmspc_ = path.substr(indexoffile + 1, path.size());
			DbElement<PayLoad> dbelem = server.getRepo().getCodeRepoDb()[nmspc_ + "::" + item];
			reply.attribute("author" + countStr, dbelem.name());
			reply.attribute("description" + countStr, dbelem.descrip());
			std::string temp_status;

			if (dbelem.payLoad().status() == open) {
				temp_status = "Open";
			}
			else if (dbelem.payLoad().status() == closing) {
				temp_status = "Closing";
			}
			else if(dbelem.payLoad().status() == closed) {
				temp_status = "Closed";
			}
			reply.attribute("status" + countStr, temp_status);
			//std::cout << "hahahah";
		}
	}
	else
	{
		std::cout << "\n  getFiles message did not define a path attribute";
	}
	return reply;
};


// Sends the Database Keys for Add Children Window
std::function<Msg(Msg)> sendDBkeys = [](Msg msg) {
	ServerProvider s;
	Server server;
	server = s.serverobj();
	DbElement<PayLoad> dbelem;
	dbelem.payLoad().value("../ServerRepository/" + msg.value("filename"));
	dbelem.payLoad().namespc(msg.value("namespace"));
	int result=server.getRepo().statusOfNewCheckinForCD(dbelem);
	std::string author = server.getRepo().getAuthor(dbelem);
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("sentdbelements");
	reply.attribute("authorlatest",author);
	reply.attribute("versionofcheckin",std::to_string(result));
	DbCore<PayLoad> db = server.getRepo().getCodeRepoDb();
	std::vector<std::string> dbkeys=db.keys();
	
	size_t count = 0;
	for (auto x : db.dbStore()) {
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("key" + countStr, x.first);
		if (x.second.children().size() != 0) {
			std::string child_number = "child" + countStr;
			// iterate of all children and add to reply message attribute
			size_t internalcount = 0;
			for (auto y : x.second.children()) {
				std::string countinternal = Utilities::Converter<size_t>::toString(++internalcount);
				reply.attribute(child_number+countinternal,y);

			}


		}
		
	}
	return reply;

};


// Sends the Database Keys for Add Children Window for automation

std::function<Msg(Msg)> sendDBkeysAuto = [](Msg msg) {
	ServerProvider s;
	Server server;
	server = s.serverobj();
	DbElement<PayLoad> dbelem;
	dbelem.payLoad().value("../ServerRepository/" + msg.value("filename"));
	dbelem.payLoad().namespc(msg.value("namespace"));

	int result = server.getRepo().statusOfNewCheckinForCD(dbelem);
	std::string author = server.getRepo().getAuthor(dbelem);

	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("sentdbelementsauto");
	reply.attribute("authorlatest", author);
	reply.attribute("versionofcheckin", std::to_string(result));
	DbCore<PayLoad> db = server.getRepo().getCodeRepoDb();
	std::vector<std::string> dbkeys = db.keys();

	size_t count = 0;
	for (auto x : db.dbStore()) {
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("key" + countStr, x.first);
		if (x.second.children().size() != 0) {
			std::string child_number = "child" + countStr;
			// iterate of all children and add to reply message attribute
			size_t internalcount = 0;
			for (auto y : x.second.children()) {
				std::string countinternal = Utilities::Converter<size_t>::toString(++internalcount);
				reply.attribute(child_number + countinternal, y);

			}


		}

	}
	return reply;

};

// Sends the Database Keys for Add Children Window for automation

std::function<Msg(Msg)> sendDBkeysAuto2 = [](Msg msg) {
	ServerProvider s;
	Server server;
	server = s.serverobj();
	DbElement<PayLoad> dbelem;
	dbelem.payLoad().value("../ServerRepository/" + msg.value("filename"));
	dbelem.payLoad().namespc(msg.value("namespace"));

	int result = server.getRepo().statusOfNewCheckinForCD(dbelem);
	std::string author = server.getRepo().getAuthor(dbelem);

	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("sentdbelementsauto2");
	reply.attribute("authorlatest", author);
	reply.attribute("versionofcheckin", std::to_string(result));
	DbCore<PayLoad> db = server.getRepo().getCodeRepoDb();
	std::vector<std::string> dbkeys = db.keys();

	size_t count = 0;

	for (auto x : db.dbStore()) {
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("key" + countStr, x.first);
		if (x.second.children().size() != 0) {
			std::string child_number = "child" + countStr;
			// iterate of all children and add to reply message attribute
			size_t internalcount = 0;
			for (auto y : x.second.children()) {
				std::string countinternal = Utilities::Converter<size_t>::toString(++internalcount);
				reply.attribute(child_number + countinternal, y);

			}


		}

	}
	return reply;

};

 // Returns Database elements with no dependencies
std::function<Msg(Msg)> browseNoParent = [](Msg msg) {


	ServerProvider s;
	Server server;
	server = s.serverobj();
	std::vector<std::string> dbkeys;
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFilesbrowse");
	DbCore<PayLoad> db = server.getRepo().getCodeRepoDb();

	for (auto dbelem : db.dbStore()) {

		if (dbelem.second.children().size() == 0) {
			dbkeys.push_back(dbelem.first);


		}
	}

	 
	size_t count = 0;
	for (auto x : dbkeys) {
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("key" + countStr, x);


	}




	return reply;

};

// Multi Browse Functionality
std::function<Msg(Msg)> multiBrowse = [](Msg msg) {


	ServerProvider s;
	Server server;
	server = s.serverobj();
	std::vector<std::string> dbkeys;

	std::vector<std::string> vec_category, vec_filename, vec_version, vec_children, vec_keys;
	if (msg.containsKey("category"))
		vec_category = browseDelimiter(msg.value("category"));
	if (msg.containsKey("filename"))
		vec_filename = browseDelimiter(msg.value("filename"));
	if (msg.containsKey("dependency"))
		vec_children = browseDelimiter(msg.value("dependency"));
	if (msg.containsKey("version"))
		vec_version = browseDelimiter(msg.value("version"));

	dbkeys=server.getRepo().MultiBrowse(vec_category,vec_filename,vec_version,vec_children);

	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("getFilesbrowse");
	

	size_t count = 0;
	for (auto x : dbkeys) {
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("key" + countStr, x);


	}




	return reply;

};






std::function<Msg(Msg)> temp = [](Msg msg) {
	Msg reply;
	if (true) {
         return reply;
	}

	//return reply;

};





int main()
{
  std::cout << "\n  Testing Server Prototype";
  std::cout << "\n ==========================";
  std::cout << "\n";
  std::cout << "Checkin Status are\n 0) : Open \n 1) Closed \n 2) Closing  ";
  Server server(serverEndPoint, "ServerPrototype");
  server.start();
  server.initializeCodeRepo();
  server.getRepo().showCodeRepoDb();
  ServerProvider sprov;
  sprov.serverobj() = server;
  std::cout << "\n  testing getFiles and getDirs methods";
  std::cout << "\n --------------------------------------";
  Repository::Files files = server.getFiles();
  show(files, "Files:");
  Dirs dirs = server.getDirs();
  show(dirs, "Dirs:");
  std::cout << "\n";
  std::cout << "\n  testing message processing";
  std::cout << "\n ----------------------------";
  server.addMsgProc("echo", echo);server.addMsgProc("getFilesbrowse", getFiles_browse);
  server.addMsgProc("getDirsbrowse", getDirs_browse);
  server.addMsgProc("getDirsmetadata", getDirs_metadata);server.addMsgProc("getFilesmetadata", getFiles_metadata);
  server.addMsgProc("getDirscheckout", getDirs_checkout);
  server.addMsgProc("getFilescheckout", getFiles_checkout);
  server.addMsgProc("getDirscheckin", getDirs_checkin);server.addMsgProc("getFilescheckin",getFiles_checkin);
  server.addMsgProc("getDirsfile", getDirs_file);server.addMsgProc("getFilesfile", getFiles_file);
  server.addMsgProc("serverQuit", echo);server.addMsgProc("connect", connectFromClient);
  server.addMsgProc("sendtorepo",sendToRepo);server.addMsgProc("sendtoclient",sendFileToClient);
  server.addMsgProc("sendtoclientmetadata", sendFileToClientMetadata);server.addMsgProc("checkout",checkout);
  server.addMsgProc("viewfile", viewFile);server.addMsgProc("filesentack",fileReceivedAck);
  server.addMsgProc("createfolder",createFolder);server.addMsgProc("senddblist",sendDBkeys);
  server.addMsgProc("senddblistauto", sendDBkeysAuto);server.addMsgProc("senddblistauto2", sendDBkeysAuto2);
  server.addMsgProc("browsenoparent", browseNoParent);server.addMsgProc("multibrowse",multiBrowse);
  server.processMessages();std::cout << "\n  press enter to exit";
  std::cin.get();
  std::cout << "\n";
  Msg msg(serverEndPoint, serverEndPoint);
  msg.command("echo");
  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

