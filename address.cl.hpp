////////////////////////////////////////////////////////////
#pragma once
namespace std{}; using namespace std;
////////////////////////////////////////////////////////////
#include "cloth_classdef.cl.hpp"
#include "address.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace Gnaf{
class AddieCompo{
   public:
   //-------------------------------------------------------
   bool filled = false;
   int valuenum = -1;
   string firstvalpostfix;
   char connector = 0;
   int secvaluenum = -1;
   string secvalpostfix;
   string value;
   string postfixlong;
   string postfix;
   //-------------------------------------------------------
      operator bool() { return filled; }
   AddieCompo();
   AddieCompo(const string& firststr, const string& secondstr);
   void printFirstPart();
   string stringifyFirstPart();
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class Address{
   public:
   //-------------------------------------------------------
   AddieCompo unitnumber;
   AddieCompo floorlevel;
   AddieCompo lot;
   AddieCompo propnumber;
   AddieCompo streetname;
   AddieCompo deliverytype;
   AddieCompo locality;
   AddieCompo state;
   AddieCompo postcode;
   AddieCompo country;
   string error_remainder;
   //-------------------------------------------------------
   string toStandardisedFormat();
   void print();
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
}; // end namespace Gnaf
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

