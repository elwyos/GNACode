////////////////////////////////////////////////////////////
#pragma once
namespace std{}; using namespace std;
////////////////////////////////////////////////////////////
#include "cloth_classdef.cl.hpp"
#include "../shing/easyparser.hpp"
#include "address.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace Gnaf{
class AddieParser{
   public:
   //-------------------------------------------------------
   EasyParser offiparsa;
   Address ret;
   //-------------------------------------------------------
   AddieParser(const shringer& leshring);
   void performParse();
   tuple<string,string,bool> maybeParseCodedWords(EasyParser& parsa, const vector<pair< string, string>>& codes);
   bool parseEsotericNumber(EasyParser& _parsa, AddieCompo& ret);
   bool tryParseUnitNumber();
   bool tryParseFloorLevel();
   bool tryParseHousePropertyNumber();
   bool tryParseLot();
   bool parseStreetNameAndType();
   bool parseDeliveryType();
   bool parseLocality();
   bool parseStateTerritory();
   bool parsePostCode();
   bool parseCountry();
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
}; // end namespace Gnaf
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

