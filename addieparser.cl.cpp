////////////////////////////////////////////////////////////
#include "address.cl.hpp"
#include "gnaf.cl.hpp"
#include "../shing/neocontext.hpp"
#include "../shing/strext.hpp"
#include "addieparser.cl.hpp"
#include "addieparser.ceeferinc.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace Gnaf{
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
AddieParser::AddieParser(const shringer& leshring):
   offiparsa(leshring)
{
   offiparsa.caseless();
   performParse();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void AddieParser::performParse(){
   bool phase15_reached = false;
   bool phase2_reached = false;
   bool phase3_reached = false;
   while(offiparsa--.hasChar()){
      bool gotit = false; // did it get anything on this pass?
      if(!phase15_reached){
         if(!gotit && !ret.unitnumber){
            gotit = tryParseUnitNumber();
         }
         if(!gotit && !ret.floorlevel){
            gotit = tryParseFloorLevel();
         }
      }
      if(!gotit && !ret.propnumber){
         gotit = tryParseHousePropertyNumber();
         if(gotit){
            phase15_reached = true;
         }
      }
      if(!gotit && !ret.lot){
         gotit = tryParseLot();
         if(gotit){
            phase15_reached = true;
         }
      }
      if(!gotit && (ret.propnumber || ret.lot) && !ret.streetname){
         gotit = parseStreetNameAndType();
         if(gotit){
            phase2_reached = true;
         }
      }
      if(!gotit && ret.streetname){
         if(!gotit && !ret.locality){
            gotit = parseLocality();
            if(gotit){
               phase3_reached = true;
            }
         }
         if(!gotit && !ret.state){
            gotit = parseStateTerritory();
            if(gotit){
               phase3_reached = true;
            }
         }
         if(!gotit && !ret.postcode){
            gotit = parsePostCode();
            if(gotit){
               phase3_reached = true;
            }
         }
      }
      if(gotit){
         if(offiparsa-- << ','){
         }
      }
      else {
         break; // nope, we failed...
      }
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
tuple<string,string,bool> AddieParser::maybeParseCodedWords(EasyParser& parsa, const vector<pair< string, string>>& codes){
   parsa--;
   sloopvec(codes, moo)
      if(parsa << moo.first){
         return tuple<string, string, bool>(moo.first, moo.second, false);
      }
      if(parsa << moo.second){
         if((parsa << '.')){
         }
         return tuple<string, string, bool>(moo.first, moo.second, true);
      }
   sloopend
   return tuple<string, string, bool>(string(), string(), false);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::parseEsotericNumber(EasyParser& _parsa, AddieCompo& ret){
   EasyParser parsa = _parsa;
   {
      EasyParser tempa = _parsa;
      string wholelabel = tempa--.parseLabel();
      if(wholelabel.size() == 1){
         ret.firstvalpostfix = wholelabel;
         _parsa = tempa;
         return true;
      }
   }
   if(!parsa--.paNUM()){
      return false;
   }
   ret.valuenum = parsa.parseNumber();
   if(parsa.peekingAtAlphabet()){
      ret.firstvalpostfix = string(1, parsa.getChar());
   }
   EasyParser beforeconnector = parsa;
   gplaicht(parsing_ranged_address);
   if(parsa-- << '-' && parsing_ranged_address){
      ret.connector = '-';
   }
   else {
      _parsa = parsa;
      return true;
   }
   if(!parsa--.paNUM()){
      ret.connector = 0;
      _parsa = beforeconnector;
      return true;
   }
   ret.secvaluenum = parsa--.parseNumber();
   if(parsa.peekingAtAlphabet()){
      ret.secvalpostfix = string(1, parsa.getChar());
   }
   _parsa = parsa;
   return true;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::tryParseUnitNumber(){
   EasyParser parsa = offiparsa;
   {
      EasyParser tempa = offiparsa;
      AddieCompo compo;
      if(parseEsotericNumber(tempa, compo)){
         if(tempa-- << '/'){
            compo.postfixlong = "unit";
            compo.postfix = "u";
            offiparsa = tempa;
            return true;
         }
      }
   }
   static vector<pair<string, string> > validcodes = {
         { "apartment", "apt" },
         { "factory", "fy" },
         { "flat", "f" },
         { "marine berth", "mb" },
         { "office", "off" },
         { "room", "rm" },
         { "house", "hse" },
         { "penthouse", "pths" },
         { "shed", "shed" },
         { "shop", "shop" },
         { "site", "site" },
         { "studio", "stu" },
         { "stall", "sl" },
         { "suite", "se" },
         { "unit", "u" },
         { "villa", "vlla" },
         { "warehouse", "we" },         
         { "townhouse", "tnhs" },         
      };
   AddieCompo compo;
   if(parsa--.paLS()){
      auto [longname, shortened, abbreved] = maybeParseCodedWords(parsa, validcodes);
      compo.postfixlong = longname;
      compo.postfix = shortened;
      if(shortened == "f" || shortened == "apt"){
         compo.postfixlong = "unit";
         compo.postfix = "u"; // interchangeable, so..
      }
      if(longname.size() != 0){
         if(parseEsotericNumber(parsa, compo)){
            ret.unitnumber = compo;
            ret.unitnumber.filled = true;
            offiparsa = parsa;
            return true;
         }
      }
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::tryParseFloorLevel(){
   EasyParser parsa = offiparsa;
   static vector<pair<string, string> > validcodes = {
         { "basement", "b" },
         { "floor", "fl" },
         { "ground floor", "g" },
         { "level", "l" },
         { "lower ground floor", "lg" },
         { "mezzanine", "m" },
         { "upper ground floor", "ugf" },
      };
   AddieCompo compo;
   if(parsa--.paLS()){
      auto [longname, shortened, abbreved] = maybeParseCodedWords(parsa, validcodes);
      compo.postfixlong = longname;
      compo.postfix = shortened;
      if(longname.size() != 0){
         if(parsa-- << ","){
         }
         if(shortened == "fl" || shortened == "l"){
            compo.postfixlong = "level";
            compo.postfix = "l";
            if(parseEsotericNumber(parsa, compo)){
               ret.floorlevel = compo;
               ret.floorlevel.filled = true;
               offiparsa = parsa;
               return true;
            }
            else {
               return false; // we need a number bro
            }
         }
         offiparsa = parsa;
         return true; // no first component
      }
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::tryParseHousePropertyNumber(){
   EasyParser parsa = offiparsa;
   AddieCompo compo;
   splaicht(parsing_ranged_address);
   if(parseEsotericNumber(parsa, compo)){
      ret.propnumber = compo;
      ret.propnumber.filled = true;
      offiparsa = parsa;
      return true;
   }
   else {
      return false; // we need a number bro
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::tryParseLot(){
   EasyParser parsa = offiparsa;
   if(parsa-- << "lot"){
      AddieCompo compo;
      if(parseEsotericNumber(parsa, compo)){
         compo.postfixlong = "lot";
         compo.postfix = "lot";
         ret.lot = compo;
         ret.lot.filled = true;
         offiparsa = parsa;
         return true;
      }
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::parseStreetNameAndType(){
   static vector<pair<string, string> > validcodes;
   {static bool donezah = false; if(!donezah){
      Strext lestrext;
      lestrext.loadFile("bundle/street_abbrevs.strext");
      sloopvec(lestrext, moo)
         vector<string> words;
         __splitOn(moo.tag, ' ', words);
         if(words.size() >= 2){
            string abbrever = __toLower(words.back());
            words.pop_back();
            string longform = __toLower(Morestring::unsplit(words, ' '));
            validcodes.push_back(pair<string, string>(longform, abbrever));
         }
      sloopend
   ;donezah = true;}}
   EasyParser parsa = offiparsa;
   string name_buildup;
   while(parsa--.paLS()){
      if(name_buildup.size() != 0){
         name_buildup.append(1, ' ');
      }
      name_buildup.append(__toLower(parsa.parseLabel()));
      auto [longname, shortened, abbreved] = maybeParseCodedWords(parsa, validcodes);
      if(shortened.size() != 0){
         AddieCompo compo;
         compo.value = name_buildup;
         compo.postfixlong = longname;
         compo.postfix = shortened;
         ret.streetname = compo;
         ret.streetname.filled = true;
         offiparsa = parsa;
         return true;
      }
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::parseDeliveryType(){
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::parseLocality(){
   EasyParser parsa = offiparsa;
   gnaf->syncLocalityMap();
   auto lepath = parsa.parseTriePathCaseless(gnaf->locality_trie);
   if(lepath.isvalued()){
      string localityname = __toLower(lepath.getval()->name);
      ret.locality.value = localityname; // no postifix
      ret.locality.filled = true;
      offiparsa = parsa;
      return true;
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::parseStateTerritory(){
   static vector<pair<string, string> > validcodes = {
         { "australian capital territory", "act" },
         { "new south wales", "nsw" },
         { "victoria", "vic" },
         { "queensland", "qld" },
         { "tasmania", "tas" },
         { "south australia", "sa" },
         { "northern territory", "nt" },
         { "western australia", "wa" },
      };
   EasyParser parsa = offiparsa;
   auto [longname, shortened, abbreved] = maybeParseCodedWords(parsa, validcodes);
   if(shortened.size() != 0){
      ret.state.value = shortened; // just use the short state code
      ret.state.filled = true;
      offiparsa = parsa;
      return true;
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::parsePostCode(){
   EasyParser parsa = offiparsa;
   string numstr = parsa--.parseNumberString();
   if(numstr.size() == 4){
      ret.postcode.value = numstr;
      ret.postcode.filled = true;
      offiparsa = parsa;
      return true;
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddieParser::parseCountry(){
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
}; // end namespace Gnaf
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

