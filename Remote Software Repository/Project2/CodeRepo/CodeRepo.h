#pragma once




/*
/////////////////////////////////////////////////////////////////////////////////////
// CodeRepo.h - Provides the core Repository functions                             //
// ver 1.0                                                                         //
// Language:    C++, Visual Studio 2017                                           //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018                  //
////////////////////////////////////////////////////////////////////////////////////


/*
Package Operations:
* -------------------
* This package provides 1 class:
*
- CodeRepo : Template class to demonstrate CodeRepository operations like Checkin,Checkout,Browse,Version

The member functions are :

  1)   checkIn(DbElement<T> dbelem) :
         Checks in a File whose information is stored in DbElement.

  2)   checkOut(DbElement<T> dbelem,int version_=0) :
          Checks out a latest version of the file whose information is stored in DbElement

  3)   showCodeRepoDb() :
         Displays the CodeRepository Database

  4)   fileStatusVersion(int version, DbElement<T> dbelem) :
          Decides what should be the version of the checkin file , based on the status of the latest file
		  in the Code Repository


  5)    listOfDependentFiles(const std::string& key) :
           returns a list of files which the input file "key" is dependent on


  6)    closeLatestFile(DbElement<T> dbelem,int version) :
            Function to handle the condtion where the checkin file wants to be in closed state


  7)    removeFromParent(const std::string& key) :
            removes the "key" from each parent map entry 


  8)    removeParent(const std::string& first,const std::string& second) :
             removes "second" from parent[first] vector entry

  9)   writeDatabaseToXml(const std::string& path) :
         Writes the Repository database to xml file


  10)   ReadDatabaseFromXml(const std::string& path)
           Reads the database xml and stores in Repository Database

  11)   BrowseByFile(const std::string& filename,int version=0) :
          Browses a specific filename with given version


  12)   writeChildToXml(const std::string& path)  :
           writes the child dependency map to xml file

  13)    readChildFromXml(const std::string& path) :
           reads the xml file and stores the content in child dependency map

  14)   writeParentToXml(const std::string& path) :
           writes the child dependency map to xml file

  15)   readParentFromXml(const std::string& path) :
           reads the xml file and stores the content in parent dependency map

  16)  BrowseByCategory(std::vector<std::string>& category_name) :
              Browses the database for a given vector of categories 

  17)  getCodeRepoDb() :
           returns the Code Repository database



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
* ver 1.0 : 1st March 2018

*/

#include<iostream>
#include<string>

#include <algorithm>    
#include <vector> 
#include <unordered_map>

#include "../FileMgr/FileMgr.h"
#include "../NoSQLDB/Executive/NoSqlDb.h"
#include "../NoSQLDB/XmlDocument/XmlParser/XmlParser.h"
#include "../NoSQLDB/PayLoad/PayLoad.h"
#include "../CheckIn/CheckIn.h"
#include "../CheckOut/CheckOut.h"
#include "../Version/Version.h"
#include "../Browse/Browse.h"

using namespace NoSqlDb;
using xyz = std::string;
using abcd = std::vector<std::string>;



template<typename T>
class CodeRepo {

private:
	static std::string rootPath;
	static std::string stageForBrowse;
	static std::string localRepo;
	static std::string localSendForRepo;
	DbCore<T> codeRepoDb;
	std::unordered_map<std::string, std::vector<std::string>> parent;
	std::unordered_map<std::string, std::vector<std::string>> child;
	
public :

	int statusOfNewCheckinForCD(DbElement<T> dbelem);
	xyz getAuthor(DbElement<T> dbelem);
	void checkIn(DbElement<T> dbelem);
	void checkOut(DbElement<T> dbelem,int version_=0);
	void showCodeRepoDb();
	int fileStatusVersion(int version, DbElement<T> dbelem);
	int fileStatusVersionForCD(int version, DbElement<T> dbelem);
	std::vector<std::string> listOfDependentFiles(const std::string& key);
	abcd MultiBrowse(const std::vector<std::string> & category_name, const std::vector<std::string> & filename, const std::vector<std::string> & version, const std::vector<std::string> & children);
	void closeLatestFile(DbElement<T> dbelem,int version);
	void removeFromParent(const std::string& key);
	void removeParent(const std::string& first,const std::string& second);
	void writeDatabaseToXml(const std::string& path);
	void ReadDatabaseFromXml(const std::string& path);
	void BrowseByFile(const std::string& filename,int version=0);
	void writeChildToXml(const std::string& path);
	void readChildFromXml(const std::string& path);
	void writeParentToXml(const std::string& path);
	void readParentFromXml(const std::string& path);
	DbCore<T> BrowseByCategory(const std::vector<std::string>& category_name);
	DbCore<T> BrowseByFileName(const std::vector<std::string> & filename,const DbCore<T>& db);
	DbCore<T> BrowseByVersion(const std::vector<std::string> & version, const DbCore<T>& db);
	DbCore<T> BrowseByChildren(const std::vector<std::string> & children, const DbCore<T>& db);
	
	DbCore<T>& getCodeRepoDb() {

		return codeRepoDb;
	}
};




template<typename T>
inline xyz CodeRepo<T>::getAuthor(DbElement<T> dbelem) {

	T t = dbelem.payLoad();
	int version = Version<T>::getVersion(this->codeRepoDb, dbelem);

	std::string namespace_ = t.namespc();
	std::string filepath_ = t.value();
	size_t indexoffile = filepath_.find_last_of("/");
	std::string filename_ = filepath_.substr(indexoffile + 1, filepath_.size());
	std::string finalkey = namespace_ + "::" + filename_ + "." + std::to_string(version);

	if (version > 0) {
		DbElement<T> tempelem = codeRepoDb[finalkey];
		return tempelem.name();
	}

	return "empty";



}

template<typename T>
inline int CodeRepo<T>::statusOfNewCheckinForCD(DbElement<T> dbelem) {

	T t = dbelem.payLoad();
	int version = Version<T>::getVersion(this->codeRepoDb, dbelem);
	int finalversion = fileStatusVersionForCD(version,dbelem);

	return finalversion;


}

template<typename T>
inline int CodeRepo<T>::fileStatusVersionForCD(int version, DbElement<T> dbelem)
{

	T t = dbelem.payLoad();
	std::string namespace_ = t.namespc();
	std::string filepath_ = t.value();
	size_t indexoffile = filepath_.find_last_of("/");
	std::string filename_ = filepath_.substr(indexoffile + 1, filepath_.size());
	std::string finalkey = namespace_ + "::" + filename_ + "." + std::to_string(version);


	if (version != 0) {

		DbElement<T> temp = codeRepoDb[finalkey];
		T temppayload = temp.payLoad();
		if (temppayload.status() == open) {

			return version;
		}
		else if (temppayload.status() == closed) {

			return version + 1;
		}
		else if (temppayload.status() == closing) {
			return -1;
		}


	}
	else {

		return 1;
	}



	return 0;

}



template<typename T>
inline void CodeRepo<T>::checkIn(DbElement<T> dbelem)
{
	
	T t = dbelem.payLoad();
	int version=Version<T>::getVersion(this->codeRepoDb,dbelem);
	
	int finalversion=fileStatusVersion(version,dbelem);

	// code to check what should be entry of the final version
	if (finalversion == -1) {
		std::cout << "Latest File is in closing state....cannot checkIN" << std::endl;
	}
	else {
		closeLatestFile(dbelem, finalversion);

		std::string namespace_ = t.namespc();
		std::string filepath_ = t.value();
		size_t indexoffile = filepath_.find_last_of("/");
		std::string filename_ = filepath_.substr(indexoffile + 1, filepath_.size());
		std::string finalkey = namespace_ + "::" + filename_ + "." + std::to_string(finalversion);
		
		std::string localpath = CodeRepo<T>::localRepo + "/" + filename_;
		CheckIn<T>::savingFileinCodeRepo(localpath, finalkey, CodeRepo<T>::rootPath);

	}
	
	
}

template<typename T>
inline void CodeRepo<T>::checkOut(DbElement<T> dbelem,int version_)
{
	
	if (version_ == 0) {

		T t = dbelem.payLoad();
		int version = Version<T>::getVersion(this->codeRepoDb, dbelem);
		

		std::string namespace_ = t.namespc();
		std::string filepath_ = t.value();
		size_t indexoffile = filepath_.find_last_of("/");
		std::string filename_ = filepath_.substr(indexoffile + 1, filepath_.size());
		//std::string finalkey = namespace_ + "::" + filename_ + "." + std::to_string(version);
		std::string finalkey = namespace_ + "::" + filename_;

		

		std::string dstdirectory = filepath_.substr(0, indexoffile + 1);

		std::vector<std::string> dependentlist = listOfDependentFiles(finalkey);
		for (auto x : dependentlist) {
			CheckOut<T>::sendFilestoClient(CodeRepo<T>::localSendForRepo, x, CodeRepo<T>::rootPath);
		}


	}
	
	


}

template<typename T>
inline void CodeRepo<T>::showCodeRepoDb()
{
	std::ostream& out = std::cout;
	std::cout << "\n\n";
	Utilities::title("Repository Database :");
	showDb(codeRepoDb);
	Utilities::title("\n\nPayload Content :");
	std::cout << "\n\n";
	PayLoad::showDb(codeRepoDb,out);
}

template<typename T>
inline int CodeRepo<T>::fileStatusVersion(int version, DbElement<T> dbelem)
{
	
	T t = dbelem.payLoad();
	std::string namespace_ = t.namespc();
	std::string filepath_ = t.value();
	size_t indexoffile = filepath_.find_last_of("/");
	std::string filename_ = filepath_.substr(indexoffile + 1, filepath_.size());
	std::string finalkey = namespace_ + "::" + filename_ + "." + std::to_string(version);

	if (version != 0) {

		DbElement<T> temp = codeRepoDb[finalkey];
		T temppayload = temp.payLoad();
		if (temppayload.status() == open) {
			
			std::string finalkey = namespace_ + "::" + filename_ + "." + std::to_string(version);
			codeRepoDb[finalkey] = dbelem;
			// replace current version
			return version;
		}
		else if (temppayload.status() == closed) {
			
			// add the new version to Database
			std::string finalkeynewversion = namespace_ + "::" + filename_ + "." + std::to_string(version+1);
			codeRepoDb[finalkeynewversion] = dbelem;
			return version + 1;
		}
		else if (temppayload.status() == closing) {
			return -1;
		}


	}
	else {
		
		std::string finalkeynewversion = namespace_ + "::" + filename_ + "." + std::to_string(1);
		codeRepoDb[finalkeynewversion] = dbelem;
		return 1;
	}
	
  
	return 0;
}

template<typename T>
inline std::vector<std::string> CodeRepo<T>::listOfDependentFiles(const std::string & key)
{
	std::vector<std::string> dependentlist;
	dependentlist.push_back(key);

	// logic to add dependent files to the dependentlist......

	for (size_t i = 0;i < dependentlist.size();i++) {

		for (auto x : codeRepoDb[dependentlist[i]].children()) {

			auto it = find(dependentlist.begin(), dependentlist.end(),x);
			if (it == dependentlist.end()) {
				
				dependentlist.push_back(x);
			}


		}
	}

	return dependentlist;
}

template<typename T>
inline void CodeRepo<T>::closeLatestFile(DbElement<T> dbelem, int version)
{
	int flag = 0;
	T t = dbelem.payLoad();
	

	std::string namespace_ = t.namespc();
	std::string filepath_ = t.value();
	size_t indexoffile = filepath_.find_last_of("/");
	std::string filename_ = filepath_.substr(indexoffile + 1, filepath_.size());
	std::string finalkey = namespace_ + "::" + filename_ + "." + std::to_string(version);

	DbElement<T> temp = codeRepoDb[finalkey];
	T tempPayload = temp.payLoad();

	if (tempPayload.status() == closed) {

		if (temp.children().size() == 0) {
		
			// code to update dependents status map
		}
		else {

			for (auto x : temp.children()) {

				DbElement<T> temp_children = codeRepoDb[x];
				T tempPayload_children = temp_children.payLoad();

				if (tempPayload_children.status() == open || tempPayload_children.status()==closing) {
					//std::cout << "Dependent FIle is Open...so cannot close" << std::endl;
					// add to map the dependent entry
					parent[finalkey].push_back(x);
					child[x].push_back(finalkey);
					flag = 1;
				}

			}

		}
		// if flag ==1 , it indicates that current file cannot be closed...so change status to closing
		if (flag == 0) {
			// code to update dependents status map
			removeFromParent(finalkey);
		}
		else {
			codeRepoDb[finalkey].payLoad().status(closing);
		}

	}
}

template<typename T>
inline void CodeRepo<T>::removeFromParent(const std::string & key)
{
	

	auto it = child.find(key);
	if (it != child.end()) {

		std::vector<std::string>& childs = child[key];
		if (child[key].size() > 0) {   //  if no other child dependents , then no need for recursion



			for (auto x : childs) {

				// remove key from each childs
				removeParent(x, key);

				if (parent[x].size() == 0) {

					// change status of key x to closed from closing
					codeRepoDb[x].payLoad().status(closed);

					parent.erase(x);

					size_t indexofdot = x.find_last_of(".");
					std::string temp = x.substr(indexofdot + 1, x.size());

					std::stringstream stream(temp);
					size_t version_int = 0;
					stream >> version_int;
					closeLatestFile(codeRepoDb[x], version_int);


				}
			}

			// erase child[key] 
			child.erase(key);



		}
	}
	


}

template<typename T>
inline void CodeRepo<T>::removeParent(const std::string & first, const std::string & second)
{ 
	

	for (auto it = parent[first].begin(); it != parent[first].end();it++) {

		if (*it == second) {
			parent[first].erase(it);
			break;
		}

		
	}



}

template<typename T>
inline void CodeRepo<T>::writeDatabaseToXml(const std::string & path)
{
	std::ofstream ofs;
	ofs.open(path, std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	Persist<T> persist(codeRepoDb);
	Xml xml = persist.toXml();
	std::string xml_head = "< ? xml version = \"1.0\" encoding = \"utf-8\" ? >\n";
	xml_head = xml_head + xml;
	std::string filename = path;
	std::ofstream outfile(filename);
	outfile << xml_head << std::endl;
	outfile.close();
	

}

template<typename T>
inline void CodeRepo<T>::ReadDatabaseFromXml(const std::string & path)
{
	std::string filename = path;
	Persist<PayLoad> persist(this->codeRepoDb);

	std::ifstream ifs(filename);
	std::string xml((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	persist.fromXml(xml);
	

}

template<typename T>
inline void CodeRepo<T>::BrowseByFile(const std::string & filename, int version)
{
	if (version == 0) {

		// get the latest version
		// construct a DBelement and get the latest version
	}
	else {
		// search if the given version is there or not .....if there then return
		// else give error message

		std::string finalkey = filename + "." + std::to_string(version);
		if (codeRepoDb.contains(finalkey)) {

			//std::cout << "Key is present" << std::endl;
			std::string path=Browse<T>::browseByFileName(finalkey, CodeRepo<T>::rootPath, CodeRepo<T>::stageForBrowse);
			Browse<T>::openNotepadFile(path);
		}
		else {

			std::cout << "key is not present" << std::endl;
		}
	}


}

template<typename T>
inline void CodeRepo<T>::writeChildToXml(const std::string & path)
{
	std::ofstream ofs;
	ofs.open(path, std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	using sptr = std::shared_ptr<AbstractXmlElement>;
	sptr root = makeTaggedElement("dependency");
	for (auto x : this->child)
	{
		if (x.second.size() > 0) {

			sptr row = makeTaggedElement("row");

			sptr key = makeTaggedElement("key", x.first);
			row->addChild(key);
			sptr vec = makeTaggedElement("vectorlist");

			for (auto y : x.second)
			{
				sptr vecvalue = makeTaggedElement("vector", y);
				vec->addChild(vecvalue);
			}
			row->addChild(vec);
			root->addChild(row);

		}
		
	}

	sptr docEl = makeDocElement(root);
	std::string xml_head = "< ? xml version = \"1.0\" encoding = \"utf-8\" ? >\n";
	xml_head = xml_head + docEl->toString();
	std::string filename = path;
	std::ofstream outfile(filename);
	outfile << xml_head << std::endl;
	outfile.close();


}

template<typename T>
inline void CodeRepo<T>::readChildFromXml(const std::string & path)
{
	XmlParser parser(path);
	
	XmlDocument* pDoc = parser.buildDocument();
	using sptr = std::shared_ptr<AbstractXmlElement>;
	this->child.clear();

	//Select the root element and return all the childrens below root
	std::vector<sptr> vec = pDoc->elements("dependency").select();

	for (auto x : vec) {
		std::string key;
		for (auto y : x->children()) {
			if (y->tag() == "key")
			{
				key = y->children()[0]->value();
			}
			else
			{
				for (auto z : y->children())
				{
					this->child[key].push_back(z->children()[0]->value());
				}
			}
		}
	}


}

template<typename T>
inline void CodeRepo<T>::writeParentToXml(const std::string & path)
{

	std::ofstream ofs;
	ofs.open(path, std::ofstream::out | std::ofstream::trunc);
	ofs.close();

	using sptr = std::shared_ptr<AbstractXmlElement>;
	sptr root = makeTaggedElement("dependency");
	for (auto x : this->parent)
	{
		if (x.second.size() > 0) {

			sptr row = makeTaggedElement("row");

			sptr key = makeTaggedElement("key", x.first);
			row->addChild(key);
			sptr vec = makeTaggedElement("vectorlist");

			for (auto y : x.second)
			{
				sptr vecvalue = makeTaggedElement("vector", y);
				vec->addChild(vecvalue);
			}
			row->addChild(vec);
			root->addChild(row);

		}

	}

	sptr docEl = makeDocElement(root);
	std::string xml_head = "< ? xml version = \"1.0\" encoding = \"utf-8\" ? >\n";
	xml_head = xml_head + docEl->toString();
	std::string filename = path;
	std::ofstream outfile(filename);
	outfile << xml_head << std::endl;
	outfile.close();



}

template<typename T>
inline void CodeRepo<T>::readParentFromXml(const std::string & path)
{

	XmlParser parser(path);
	
	XmlDocument* pDoc = parser.buildDocument();
	using sptr = std::shared_ptr<AbstractXmlElement>;
	this->parent.clear();

	//Select the root element and return all the childrens below root
	std::vector<sptr> vec = pDoc->elements("dependency").select();

	for (auto x : vec) {
		std::string key;
		for (auto y : x->children()) {
			if (y->tag() == "key")
			{
				key = y->children()[0]->value();
			}
			else
			{
				for (auto z : y->children())
				{
					this->parent[key].push_back(z->children()[0]->value());
				}
			}
		}
	}


}

template<typename T>
inline DbCore<T> CodeRepo<T>::BrowseByCategory(const std::vector<std::string>& category_name)
{
	DbCore<T> temp_db;
	if (category_name.size() > 0)
	{
		for (auto y : category_name)
		{
			std::vector<std::string> keys;
			 Browse<T>::browseCategory(this->codeRepoDb, y,keys);
			if (keys.size() > 0)
			{
				for (auto x : keys)
				{
					temp_db[x] = this->codeRepoDb[x];
				}
			}
		}
	}
	return temp_db;

	
}


template<typename T>
inline DbCore<T> CodeRepo<T>::BrowseByFileName(const std::vector<std::string> & filename,const DbCore<T>& db) {

	DbCore<T> fulldb = db;
	DbCore<T> temp_db;
	if (filename.size() > 0)
	{
		for (auto y : filename)
		{
			std::vector<std::string> keys;
			Browse<T>::browseByFileName(fulldb, y, keys); //Querying DB on the basis of category.
			if (keys.size() > 0)
			{
				for (auto x : keys)
				{
					temp_db[x] = fulldb[x];
				}
			}
		}
	}
	return temp_db;


}


template<typename T>
inline DbCore<T> CodeRepo<T>::BrowseByVersion(const std::vector<std::string> & version, const DbCore<T>& db) {

	DbCore<T> fulldb=db;
	DbCore<T> temp_db;
	if (version.size() > 0)
	{
		for (auto y : version)
		{
			std::vector<std::string> keys;
			Browse<T>::browseByVersion(fulldb, y, keys); //Querying DB on the basis of category.
			if (keys.size() > 0)
			{
				for (auto x : keys)
				{
					temp_db[x] = fulldb[x];
				}
			}
		}
	}
	return temp_db;


}

template<typename T>
inline DbCore<T> CodeRepo<T>::BrowseByChildren(const std::vector<std::string> & children, const DbCore<T>& db) {

	DbCore<T> fulldb=db;
	DbCore<T> temp_db;
	if (children.size() > 0)
	{
		for (auto y : children)
		{
			std::vector<std::string> keys;
			Browse<T>::browseByChildren(fulldb, y, keys); //Querying DB on the basis of category.
			if (keys.size() > 0)
			{
				for (auto x : keys)
				{
					temp_db[x] = fulldb[x];
				}
			}
		}
	}
	return temp_db;

}




template<typename T>
//This function calss Browse by catoegory of browe class.
inline abcd CodeRepo<T>::MultiBrowse(const std::vector<std::string> & category_name,const std::vector<std::string> & filename,const std::vector<std::string> & version,const std::vector<std::string> & children) {

	DbCore<T> temp_db;
	if (category_name.size() > 0)
		temp_db = BrowseByCategory(category_name);
	else
		temp_db = this->codeRepoDb;

	if (filename.size() > 0)
		temp_db = BrowseByFileName(filename, temp_db);

	if (version.size() > 0)
		temp_db = BrowseByVersion(version, temp_db);

	if (children.size() > 0)
		temp_db = BrowseByChildren(children, temp_db);



	return temp_db.keys();


}

template<typename T>
std::string CodeRepo<T>::rootPath = "../Storage";

template<typename T>
std::string CodeRepo<T>::localRepo = "../ServerRepository";

template<typename T>
std::string CodeRepo<T>::localSendForRepo = "../SendFilesRepo";

template<typename T>
std::string CodeRepo<T>::stageForBrowse = "../stage";
