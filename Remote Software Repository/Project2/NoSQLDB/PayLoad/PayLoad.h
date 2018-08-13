#ifndef PAYLOAD_H
#define PAYLOAD_H
///////////////////////////////////////////////////////////////////////
// PayLoad.h - application defined payload                           //
// ver 1.1                                                           //
// Prasanna Dodwad, CSE687 - Object Oriented Design, Spring 2018     //
// Source : Jim Fawcett                                              //
///////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package provides a single class, PayLoad:
*  - holds a payload string and vector of categories
*  - provides means to set and access those values
*  - provides methods used by Persist<PayLoad>:
*    - Sptr toXmlElement();
*    - static PayLoad fromXmlElement(Sptr elem);
*  - provides a show function to display PayLoad specific information
*  - PayLoad processing is very simple, so this package contains only
*    a header file, making it easy to use in other packages, e.g.,
*    just include the PayLoad.h header.

  Public functions :

    std::string value() const : getter function for value
	std::string& value() : getter function for value by reference
	void value(const std::string& value) : setter function for value 

	std::string namespc() const :  getter function for namespace 
	std::string& namespc() :  getter function for namespace by reference
	void namespc(const std::string& value) :  setter function for namespace

	filestatus status() const : getter function for filestatus 
	filestatus& status() : getter function for filestatus by reference
	void status(const filestatus& value) : setter function for filestatus

	std::vector<std::string>& categories() : getter function for categories
	std::vector<std::string> categories() const  : getter function for categories by reference

	void identify(std::ostream& out = std::cout) :
	PayLoad& operator=(const std::string& val) :

	bool hasCategory(const std::string& cat) : lambda function for category

	Sptr toXmlElement();
	static PayLoad fromXmlElement(Sptr elem);

	static void showPayLoadHeaders(std::ostream& out = std::cout);
	static void showElementPayLoad(NoSqlDb::DbElement<PayLoad>& elem, std::ostream& out = std::cout);
	static void showDb(NoSqlDb::DbCore<PayLoad>& db, std::ostream& out = std::cout);



*
*  Required Files:
*  ---------------
*    PayLoad.h, PayLoad.cpp - application defined package
*    DbCore.h, DbCore.cpp
*
*  Maintenance History:
*  --------------------
*  ver 1.1 : 19 Feb 2018
*  - added inheritance from IPayLoad interface
*  Ver 1.0 : 10 Feb 2018
*  - first release
*
*/

#include <string>
#include <vector>
#include <iostream>


#include "../../NoSQLDB/Executive/NoSqlDb.h"
#include "IPayLoad.h"

///////////////////////////////////////////////////////////////////////
// PayLoad class provides:
// - a std::string value which, in Project #2, will hold a file path
// - a vector of string categories, which for Project #2, will be 
//   Repository categories
// - methods used by Persist<PayLoad>:
//   - Sptr toXmlElement();
//   - static PayLoad fromXmlElement(Sptr elem);


namespace NoSqlDb
{
	enum filestatus {
		open,
		closed,
		closing

	};

  class PayLoad : public IPayLoad<PayLoad>
  {
  public:
  
	  PayLoad() : file_status(open) {} 
    PayLoad(const std::string& val) : value_(val) {}
    static void identify(std::ostream& out = std::cout);
    PayLoad& operator=(const std::string& val)
    {
      value_ = val;
      return *this;
    }
    operator std::string() { return value_; }

    std::string value() const { return value_; }
    std::string& value() { return value_; }
    void value(const std::string& value) { value_ = value; }

	std::string namespc() const { return namespace_; }
	std::string& namespc() { return namespace_; }
	void namespc(const std::string& value) { namespace_ = value; }

	filestatus status() const { return file_status; }
	filestatus& status() { return file_status; }
	void status(const filestatus& value) { file_status = value; }



    std::vector<std::string>& categories() { return categories_; }
    std::vector<std::string> categories() const { return categories_; }

    bool hasCategory(const std::string& cat)
    {
      return std::find(categories().begin(), categories().end(), cat) != categories().end();
    }

    Sptr toXmlElement();
    static PayLoad fromXmlElement(Sptr elem);

    static void showPayLoadHeaders(std::ostream& out = std::cout);
    static void showElementPayLoad(NoSqlDb::DbElement<PayLoad>& elem, std::ostream& out = std::cout);
    static void showDb(NoSqlDb::DbCore<PayLoad>& db, std::ostream& out = std::cout);
  private:
    std::string value_;
    std::vector<std::string> categories_;
	std::string namespace_;
	filestatus file_status;
  };

  //----< show file name >---------------------------------------------

  inline void PayLoad::identify(std::ostream& out)
  {
    out << "\n  \"" << __FILE__ << "\"";
  }
  //----< create XmlElement that represents PayLoad instance >---------
  /*
  * - Required by Persist<PayLoad>
  */
  inline Sptr PayLoad::toXmlElement()
  {
    Sptr sPtr = XmlProcessing::makeTaggedElement("payload");
    XmlProcessing::XmlDocument doc(makeDocElement(sPtr));
    Sptr sPtrVal = XmlProcessing::makeTaggedElement("value",value_);
    sPtr->addChild(sPtrVal);
	Sptr sPtrNmSpc = XmlProcessing::makeTaggedElement("namespace", namespace_);
	sPtr->addChild(sPtrNmSpc);
	std::string temp_status;
	if (file_status == closed)
		temp_status = "closed";
	else if (file_status == open)
		temp_status = "open";
	else
		temp_status = "closing";
	Sptr sPtrObjectStatus = XmlProcessing::makeTaggedElement("status", temp_status);
	sPtr->addChild(sPtrObjectStatus);


    Sptr sPtrCats = XmlProcessing::makeTaggedElement("categories");
    sPtr->addChild(sPtrCats);
    for (auto cat : categories_)
    {
      Sptr sPtrCat = XmlProcessing::makeTaggedElement("category", cat);
      sPtrCats->addChild(sPtrCat);
    }
    return sPtr;
  }
  //----< create PayLoad instance from XmlElement >--------------------
  /*
  * - Required by Persist<PayLoad>
  */
  inline PayLoad PayLoad::fromXmlElement(Sptr pElem)
  {
    PayLoad pl;
    for (auto pChild : pElem->children())
    {
		std::string val;
      std::string tag = pChild->tag();
	  int sizeforcategory=pChild->children().size();
	  if (sizeforcategory != 0) {
		   val = pChild->children()[0]->value();
	  }
      
      if (tag == "value")
      {
        pl.value(val);
      }
	  if (tag == "namespace")
	  {
		  pl.namespc(val);
	  }
	  if (tag == "status")
	  {
		  if (val == "closed")
			  pl.status(closed);
		  else if (val == "open")
			  pl.status(open);
		  else
			  pl.status(closing);
	  }
	  if (tag == "categories" && sizeforcategory!=0)
      {
        std::vector<Sptr> pCategories = pChild->children();
        for (auto pCat : pCategories)
        {
          pl.categories().push_back(pCat->children()[0]->value());
        }
      }
    }
    return pl;
  }
  /////////////////////////////////////////////////////////////////////
  // PayLoad display functions

  inline void PayLoad::showPayLoadHeaders(std::ostream& out)
  {
    out << "\n  ";
    out << std::setw(10) << std::left << "Name";
    out << std::setw(45) << std::left << "Payload Value";
    out << std::setw(25) << std::left << "Payload Status";
	out << std::setw(25) << std::left << "Payload Categories";
    out << "\n  ";
    out << std::setw(10) << std::left << "--------";
    out << std::setw(40) << std::left << "--------------------------------------";
    out << std::setw(25) << std::left << "-----------------------";
	out << std::setw(25) << std::left << "-----------------------";
  }


  inline void PayLoad::showElementPayLoad(NoSqlDb::DbElement<PayLoad>& elem, std::ostream& out)
  {
    out << "\n  ";
    out << std::setw(10) << std::left << elem.name().substr(0, 8);
    out << std::setw(40) << std::left << elem.payLoad().value().substr(0, 38);
	out << std::setw(25) << std::left << elem.payLoad().status();

    for (auto cat : elem.payLoad().categories())
    {
      out << cat << " ";
    }
  }

  inline void PayLoad::showDb(NoSqlDb::DbCore<PayLoad>& db, std::ostream& out)
  {
    showPayLoadHeaders(out);
    for (auto item : db)
    {
      NoSqlDb::DbElement<PayLoad> temp = item.second;
      PayLoad::showElementPayLoad(temp, out);
    }
  }
}
#endif
