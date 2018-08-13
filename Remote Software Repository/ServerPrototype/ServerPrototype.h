#pragma once
///////////////////////////////////////////////////////////////////////
// ServerPrototype.h - Console App that processes incoming messages  //
// ver 1.0                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* ---------------------
*  Package contains one class, Server, that contains a Message-Passing Communication
*  facility. It processes each message by invoking an installed callable object
*  defined by the message's command key.
*
*  Message handling runs on a child thread, so the Server main thread is free to do
*  any necessary background processing (none, so far).
*
*  Required Files:
* -----------------
*  ServerPrototype.h, ServerPrototype.cpp
*  Comm.h, Comm.cpp, IComm.h
*  Message.h, Message.cpp
*  FileSystem.h, FileSystem.cpp
*  Utilities.h
*
*  Maintenance History:
* ----------------------
*  ver 1.0 : 3/27/2018
*  - first release
*/
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include "../CppCommWithFileXfer/Message/Message.h"
#include "../CppCommWithFileXfer/MsgPassingComm/Comm.h"
#include "../Project2/NoSQLDB/PayLoad/PayLoad.h"
#include "../Project2/ExecutiveCodeRepo/ExecutiveCodeRepo.h"
#include <windows.h>
#include <tchar.h>

using namespace NoSqlDb;

std::vector<std::string> findChildrensforh(const std::string& children) {

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

	return result;

}

namespace Repository
{
  using File = std::string;
  using Files = std::vector<File>;
  using Dir = std::string;
  using Dirs = std::vector<Dir>;
  using SearchPath = std::string;
  using Key = std::string;
  using Msg = MsgPassingCommunication::Message;
  using ServerProc = std::function<Msg(Msg)>;
  using MsgDispatcher = std::unordered_map<Key,ServerProc>;
  const std::string sendFilePathForServer = "../SendFilesRepo";
  const SearchPath storageRoot = "../Storage";  // root for all server file storage
  const MsgPassingCommunication::EndPoint serverEndPoint("localhost", 8080);  // listening endpoint
  const std::string databasePath = "../DB.xml";
  const std::string childPath = "../child.xml";
  const std::string parentPath = "../parent.xml";

  class Server
  {
  public:
	  
	Server() {};
    Server(MsgPassingCommunication::EndPoint ep, const std::string& name);
    void start();
    void stop();
    void addMsgProc(Key key, ServerProc proc);
    void processMessages();
	void initializeCodeRepo();
	void persistCodeRepo();
	Server& operator=(const Server& s);
	void sentFile(MsgPassingCommunication::Message msg);
	void checkoutFile(MsgPassingCommunication::Message msg);
	void browseSend(MsgPassingCommunication::Message msg);
    void postMessage(MsgPassingCommunication::Message msg);
    MsgPassingCommunication::Message getMessage();
    static Dirs getDirs(const SearchPath& path = storageRoot);
    static Files getFiles(const SearchPath& path = storageRoot);
	CodeRepo<PayLoad>& getRepo() { return coderepo; }
	CodeRepo<PayLoad> getRepo() const { return coderepo; }
  private:
	CodeRepo<PayLoad> coderepo;
    MsgPassingCommunication::Comm comm_;
    MsgDispatcher dispatcher_;
    std::thread msgProcThrd_;

	
  };
  //----< initialize server endpoint and give server a name >----------

  inline Server::Server(MsgPassingCommunication::EndPoint ep, const std::string& name)
    : comm_(ep, name) {}

  //----< start server's instance of Comm >----------------------------

  inline void Server::start()
  {
    comm_.start();
  }
  //----< stop Comm instance >-----------------------------------------

  inline void Server::stop()
  {
    if(msgProcThrd_.joinable())
      msgProcThrd_.join();
    comm_.stop();
  }
  //----< pass message to Comm for sending >---------------------------


  inline void Server::checkoutFile(MsgPassingCommunication::Message msg) {

	  std::vector<std::string> res = findChildrensforh(msg.value("totalfiles"));

	  for (auto x : res) {

		  MsgPassingCommunication::Message reply;
		  reply = msg;
		  reply.attribute("file",x);
		  reply.attribute("filename",x);
		  postMessage(reply);

		  MsgPassingCommunication::Message sentreply;
		  sentreply.to(msg.to());
		  sentreply.from(msg.from());
		  sentreply.attribute("tab", msg.value("tab"));
		  sentreply.attribute("content", "Sent file to client...!!");
		  sentreply.command("sentfiletoclient");
		  postMessage(sentreply);


	  }
	  
	  

  }
  inline void Server::browseSend(MsgPassingCommunication::Message msg) {

	  MsgPassingCommunication::Message reply;
	  reply = msg;
	  postMessage(reply);

	  MsgPassingCommunication::Message sentreply;
	  sentreply.to(msg.to());
	  sentreply.from(msg.from());
	  sentreply.attribute("tab","browse");
	  sentreply.attribute("filename",msg.value("filename"));
	  sentreply.attribute("content", "Sent file to client...!!");
	  sentreply.command("sentfiletoclient");
	  postMessage(sentreply);

  }

  inline void Server::sentFile(MsgPassingCommunication::Message msg) {

	  if (msg.containsKey("file")) {
		  msg.remove("file");
		  msg.attribute("tab", msg.value("tab"));
		  msg.attribute("content","Sent file to client...!!");
		  msg.command("sentfiletoclient");
		  postMessage(msg);
	  }

  }

  inline void Server::initializeCodeRepo() {

	  coderepo.ReadDatabaseFromXml(databasePath);
	  coderepo.readChildFromXml(childPath);
	  coderepo.readParentFromXml(parentPath);
  }

  inline void Server::persistCodeRepo() {

	  coderepo.writeDatabaseToXml(databasePath);
	  coderepo.writeChildToXml(childPath);
	  coderepo.writeParentToXml(parentPath);
  }

  inline Server& Server::operator=(const Server& s) {

	  coderepo = s.coderepo;
	  //dispatcher_ = s.dispatcher_;
	  return *this;

  }


  inline void Server::postMessage(MsgPassingCommunication::Message msg)
  {
    comm_.postMessage(msg);
  }
  //----< get message from Comm >--------------------------------------

  inline MsgPassingCommunication::Message Server::getMessage()
  {
    Msg msg = comm_.getMessage();
    return msg;
  }
  //----< add ServerProc callable object to server's dispatcher >------

  inline void Server::addMsgProc(Key key, ServerProc proc)
  {
    dispatcher_[key] = proc;
  }
  //----< start processing messages on child thread >------------------

  inline void Server::processMessages()
  {
    auto proc = [&]()
    {
      if (dispatcher_.size() == 0)
      {
        std::cout << "\n  no server procs to call";
        return;
      }
      while (true)
      {
        Msg msg = getMessage();
		std::cout << "\n  received message: " << msg.command() << " from " << msg.from().toString();
        if (msg.containsKey("verbose"))
        {
          std::cout << "\n";
          msg.show();
        }
        if (msg.command() == "serverQuit")
          break;
		Msg reply = dispatcher_[msg.command()](msg);
        if (msg.to().port != msg.from().port)  // avoid infinite message loop
        {
			if (reply.command() == "checkoutfile") 
				checkoutFile(reply);
			else if (reply.command()=="sendingfiletobrowse") 
				browseSend(reply);
			else 
				postMessage(reply);
		  //sentFile(reply);
		  if (!msg.containsKey("file")) {
			  msg.show();
		  }
		  if (!reply.containsKey("file")) {
			  reply.show();
		  }
          
        }
        else
          std::cout << "\n  server attempting to post to self";
      }
      std::cout << "\n  server message processing thread is shutting down";
    };
    std::thread t(proc);
    //SetThreadPriority(t.native_handle(), THREAD_PRIORITY_HIGHEST);
    std::cout << "\n  starting server thread to process messages";
    msgProcThrd_ = std::move(t);
  }
}