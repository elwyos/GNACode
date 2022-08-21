//////////////////////////////////////////////////////////////////////////////////////////
#include "easyparser.hpp"
#include <istream>
#include <string>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "morestring.hpp"
#include "assert.hpp"
#include "../shing/oust.hpp"
#include "serror.hpp"
#include "context.hpp"
#include "membuf.hpp"
#include "once.hpp"
#include "shingdate.hpp"
//#include "charset.hpp"
#include "fixedmath.hpp"
//#include "attstring.hpp"
#include "errors.hpp"
#include "tilemath.hpp"
#include "coordrange.hpp"
#include "exit.hpp"
#include "utf8.h"
#include <cwctype>
//////////////////////////////////////////////////////////////////////////////////////////
#include "moremath.hpp"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
using namespace std;
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool easyparser_is_apostrophe(int thechar){
   switch(thechar){
      case 8219:
      case 8218:
      case 8217:
      case 8216:
      case '\'':
         return true;
   }
   return false;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
namespace Shing{
//////////////////////////////////////////////////////////////////////////////////////////
string PhoneNumber::standardString() const{
   if(has_plus){
      return "+" + numbers;
   }
   return numbers;
}
//////////////////////////////////////////////////////////////////////////////////////////
AnimRange::AnimRange(){
   // nothing
}
//////////////////////////////////////////////////////////////////////////////////////////
AnimRange::AnimRange(const shringer& whatever){
   name = whatever.toString();
}
//////////////////////////////////////////////////////////////////////////////////////////
AnimRange::AnimRange(const shringer& whatever, flxoat ztostarter, flxoat ztoender){
   name = whatever.toString();
   zto_start = ztostarter;
   zto_end = ztoender;
}
//////////////////////////////////////////////////////////////////////////////////////////
AnimRange::AnimRange(flxoat ztostarter, flxoat ztoender){
   zto_start = ztostarter;
   zto_end = ztoender;
}
//////////////////////////////////////////////////////////////////////////////////////////
void AnimRange::oustprint(){
   oustoust << name;
   if(zto_start != 0_flx || zto_end != 1_flx){
      oustoust[240] << " (";
      oustoust[196] << zto_start.fracstr();
      oustoust[220] << " - ";
      oustoust[112] << zto_end.fracstr();
      oustoust[240] << ")";
   }
   
   if(!dur.isNegative()){
      oustoust[250] << " " >> 220 << dur.fracstr () >> 250 << "f";
   }
   
   if(!speed.isNegative()){
      oustoust[250] << " " >> 196 << speed.fracstr () >> 250 << "x";
   }
   
}
//////////////////////////////////////////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
IntOrFloat::IntOrFloat(){
   isint = true;
   intret = 0;
   floatret = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
IntOrFloat::IntOrFloat(int theint){
   isint = true;
   intret = theint;
   floatret = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
IntOrFloat::IntOrFloat(float thefloat){
   isint = false;
   intret = 0;
   floatret = thefloat;
}
//////////////////////////////////////////////////////////////////////////////////////////
float IntOrFloat::asfloat(){
   if(isint)
      return intret;
   return floatret;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//EasyParser::EasyParser(Shing::Membuf& membuf){
//   initCharStar(membuf.c_str(), membuf.size());
//}
//////////////////////////////////////////////////////////////////////////////////////////
//EasyParser::EasyParser(Shing::Membuf* membuf){
//   initCharStar(membuf->c_str(), membuf->size());
//}
//////////////////////////////////////////////////////////////////////////////////////////
//EasyParser::EasyParser(const string& themoo){
//   initCharStar(themoo.c_str(), themoo.size());
//}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser::EasyParser(const shringer& shring, Shing::Attmeta * themeta){
   initCharStar(shring.c_str(), shring.size(), themeta);
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser::EasyParser(const char * thestr, int64_t thelength, Shing::Attmeta * themeta){
   initCharStar(thestr, thelength, themeta);
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser::EasyParser(const string& themoo, int64_t offset, Shing::Attmeta * themeta){
   initCharStar(themoo.c_str() + offset, themoo.size() - offset, themeta);
}
//////////////////////////////////////////////////////////////////////////////////////////
//EasyParser::EasyParser(istream& istr){
//   initIStream(istr);
//}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser::EasyParser(){
   initCharStar(0, 0, 0);
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::initCommon(){
   
   ended = false;
   _caseless = false;
   //_unicode = true; // let's true it by default and see if things break!
   _unicode = false; // let's see quickly
   _unicode_disabled = false; // if NoUnicode() func is called, unicode is disabled. for safety.
   unicode_lastaccepted = 0;
   
   bigendian = false;
   
   bunshin = false;
   bunshinfail = false;
   
   linenum = 0;
   colnum = 0;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::initCharStar(const char* theinput, int64_t thelen, Shing::Attmeta * theattmeta){
   
   Morestring::initSimpleLocale();

   input = theinput;
   curpoint = 0;
   acceptcount_unicode = 0;
   inputlength = thelen;
   attmeta = theattmeta;
   
   istre = NULL;
   lookpoint = 0;
   
   unicode_bufferendpoint = 0;
   
   initCommon();
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::exists(){
   return input != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::isNull(){
   return input == NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::lastCharNoUnicode(int64_t theindex){
   assert(input != NULL);
   
   // _unicode_disabled = true; // this does not actually disable unicode, because this func doesn't really change much in state
   
   int64_t checkindex = curpoint + theindex - 1;
   
   if(checkindex < 0)
      return 0;
   
   if(checkindex >= inputlength)
      return 0;
   
   return input[checkindex];
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::lastChar(int64_t theindex){
   
   if(!_unicode)
      return lastCharNoUnicode(theindex);
      
   asserts(theindex == 0, "unicode mode lastChar only accepts 0 index!");
   
   return unicode_lastaccepted;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
shringer EasyParser::getShringerFrom(int64_t startpos){
   return getShringerFromTo(startpos, curpoint);
}
//////////////////////////////////////////////////////////////////////////////////////////
shringer EasyParser::getShringerFromTo(int64_t startpos, int64_t endpos){
   assert(input != NULL);
   
   if(startpos < 0)
      startpos = 0;
   
   endpos = min(endpos, inputlength);
   
   return shringer((char*)input + startpos, endpos - startpos);
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::getLeftovers(){
   // as string.
   // identical as parseUntilEnd, but faster
   
   assert(input != NULL);
   return string((char*)input + curpoint, inputlength - curpoint);
   
}
//////////////////////////////////////////////////////////////////////////////////////////
shringer EasyParser::getLeftoverBufferUntil(int64_t untilpos){
   assert(input != NULL);
   
   int64_t thetarg = min(untilpos, inputlength);
   if(thetarg < curpoint)
      thetarg = curpoint;
   
   return shringer((char*)input + curpoint, thetarg - curpoint);
}
//////////////////////////////////////////////////////////////////////////////////////////
shringer EasyParser::getLeftoverBuffer(){
   assert(input != NULL);
   return shringer((char*)input + curpoint, inputlength - curpoint);
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::initIStream(istream& istr){

   input = NULL;
   curpoint = 0;
   acceptcount_unicode = 0;
   inputlength = 0;
   attmeta = NULL;
   
   istre = &istr;
   lookpoint = 0;
   
   initCommon();
   
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::bigEndian(){
   bigendian = true;
   return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::caseless(bool truth){
   _caseless = truth;
   return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::preloadChars(int numtopreload){

   if(ended)
      return false;
      
   while(lookbuffer.size() < numtopreload){
      int topush = istre->get();
      if(istre->good()){
         lookbuffer.push_back(topush);
      } else {
         ended = true;
         break;
      }
   }
   
   return !ended;
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::maybeFlushLookpoint(bool force){
   if(lookpoint > 16000 || force){
      asserts(lookpoint <= lookbuffer.size(), (stroust << "huh : " << lookpoint << " -- " << lookbuffer.size()) <3);
      lookbuffer = lookbuffer.substr(lookpoint);
      lookpoint = 0;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::emptyLookBuffer(){
   lookbuffer = "";
   lookpoint = 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbBool(bool& boolret){
   static_assert(sizeof(bool) == 1);
   return absorbByte(*(int8_t*)&boolret);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbChar(char& charret){
   static_assert(sizeof(char) == 1);
   return absorbByte(*(int8_t*)&charret);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbByte(char& charret){
   static_assert(sizeof(char) == 1);
   return absorbByte(*(int8_t*)&charret);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbByte(int8_t& charret){
   int thechar = getCharNoUnicode();
   if(thechar == EOF)
      return false;
   
   charret = thechar;
   
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbUByte(uint8_t& charret){
   int thechar = getCharNoUnicode();
   if(thechar == EOF)
      return false;
   
   charret = thechar;
   
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbMCVarInt(int32_t& theint){
   
   int oldcurpoint = curpoint;
   
   int numRead = 0;
   theint = 0;
   uint8_t readed;
   do {
      bool succ = absorbUByte(readed);
      if(!succ){
         curpoint = oldcurpoint; // revert
         return false;
      }
      int value = (readed & 0b01111111);
      theint |= (value << (7 * numRead));

      numRead++;
      if (numRead > 5) {
         curpoint = oldcurpoint; // revert
         return false;
      }
   } while ((readed & 0b10000000) != 0);

   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbMCVarLong(int64_t& theint){
   int oldcurpoint = curpoint;
   
   int numRead = 0;
   theint = 0;
   uint8_t readed;
   do {
      bool succ = absorbUByte(readed);
      if(!succ){
         curpoint = oldcurpoint; // revert
         return false;
      }
      int value = (readed & 0b01111111);
      theint |= (value << (7 * numRead));

      numRead++;
      if (numRead > 10) {
         curpoint = oldcurpoint; // revert, too big
         return false;
      }
   } while ((readed & 0b10000000) != 0);

   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbShort(int16_t& theshort){
   
   _unicode_disabled = true;

   if(curpoint + 2 > inputlength)
      return false;
      
   string thestr(input + curpoint, 2);
   
   theshort = bigendian? BigEndian::byteStringToShort(thestr) : Morestring::byteStringToShort(thestr);
   
   curpoint += 2;
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbShort(uint16_t& theshort){
   
   _unicode_disabled = true;

   if(curpoint + 2 > inputlength)
      return false;
      
   string thestr(input + curpoint, 2);
   
   theshort = bigendian? BigEndian::byteStringToShort(thestr) : Morestring::byteStringToShort(thestr);
   
   curpoint += 2;
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::absorbIntAndExpect(int32_t theexected){
   int theint = 0;
   bool gotint = absorbInt(theint);
   asserts(gotint, "Expecting an int..");
   asserts(theint == theexected, "Expecting " + __toString(theexected) + ", got " + __toString(theint));
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/*
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, int32_t& value, int32_t defaultvalue){
   if((*this) << code){
      ausorb(value); return true;
   } else {
      value = defaultvalue; return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, uint32_t& value, uint32_t defaultvalue){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
      value = defaultvalue;  return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, int16_t& value, int16_t defaultvalue){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
      value = defaultvalue;  return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, float& value, float defaultvalue){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
      value = defaultvalue;  return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, uint8_t& value, uint8_t defaultvalue){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
      value = defaultvalue;  return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, int8_t& value, int8_t defaultvalue){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
      value = defaultvalue;  return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, TileIndex& value, const TileIndex& defaultvalue){
   if((*this) << code){
      ausorb(value.x); ausorb(value.y);  return true;
   } else {
      value = defaultvalue;  return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, Point2D& value, const Point2D& defaultvalue){
   if((*this) << code){
      ausorb(value.x); ausorb(value.y);  return true;
   } else {
      value = defaultvalue;  return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, Coord3D& value, const Coord3D& defaultvalue){
   if((*this) << code){
      ausorb(value.x); ausorb(value.y); ausorb(value.z); return true;
   } else {
      value = defaultvalue;  return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcrd(char code, ColorInfo& value, const ColorInfo& defaultvalue){
   if((*this) << code){
      ausorb(value._red); ausorb(value._green); ausorb(value._blue);ausorb(value._alpha); return true;
   } else {
      value = defaultvalue;  return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
*/
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, int32_t& value){
   if((*this) << code){
      ausorb(value); return true;
   } else {
      return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, uint32_t& value){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
       return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, int16_t& value){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
       return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, float& value){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
       return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, uint8_t& value){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
       return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, int8_t& value){
   if((*this) << code){
      ausorb(value);  return true;
   } else {
       return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, TileIndex& value){
   if((*this) << code){
      ausorb(value.x); ausorb(value.y);  return true;
   } else {
       return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, Point2D& value){
   if((*this) << code){
      ausorb(value.x); ausorb(value.y);  return true;
   } else {
       return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, Coord3D& value){
   if((*this) << code){
      ausorb(value.x); ausorb(value.y); ausorb(value.z); return true;
   } else {
       return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::qmcr(char code, ColorInfo& value){
   if((*this) << code){
      ausorb(value._red); ausorb(value._green); ausorb(value._blue);ausorb(value._alpha); return true;
   } else {
       return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(string& ret){
   return absorbNumberStringPair(ret);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(bool& thebool){
   return absorbBool(thebool);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(TileIndex& thetile){
   if(!absorbInt(thetile.x))
      return false;
   return absorbInt(thetile.y);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(Point2D& thepoint){
   if(!absorbFloat(thepoint.x))
      return false;
   return absorbFloat(thepoint.y);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(Coord3D& thepoint){
   if(!absorbFloat(thepoint.x))
      return false;
   if(!absorbFloat(thepoint.y))
      return false;
   return absorbFloat(thepoint.z);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(ColorInfo& thecolor){
   if(!absorbFloat(thecolor._red))
      return false;
   if(!absorbFloat(thecolor._green))
      return false;
   if(!absorbFloat(thecolor._blue))
      return false;
   return absorbFloat(thecolor._alpha);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(float& theint){
   return absorbFloat(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(double& theint){
   return absorbDouble(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(uint64_t& theint){
   return absorbUnsignedLong(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(int64_t& theint){
   return absorbLong(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(int32_t& theint){
   return absorbInt(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(uint32_t& theint){
   return absorbInt(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(uint16_t& theint){
   return absorbShort(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(int16_t& theint){
   return absorbShort(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(uint8_t& theint){
   return absorbUByte(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(int8_t& theint){
   return absorbByte(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::ausorb(char& theint){
   return absorbChar(theint);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbInt(int32_t& theint){

   _unicode_disabled = true;

   if(curpoint + 4 > inputlength)
      return false;
      
   string thestr(input + curpoint, 4);
   
   theint = bigendian? BigEndian::byteStringToInt(thestr) : Morestring::byteStringToInt(thestr);
   
   curpoint += 4;
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbInt(uint32_t& theint){

   _unicode_disabled = true;

   if(curpoint + 4 > inputlength)
      return false;
      
   string thestr(input + curpoint, 4);
   
   theint = bigendian? BigEndian::byteStringToInt(thestr) : Morestring::byteStringToInt(thestr);
   
   curpoint += 4;
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
shringer EasyParser::absorbBuffer(int segmentsize){
   
   if(curpoint + segmentsize > inputlength)
      return shringer();
   shringer ret = shringer(input + curpoint, segmentsize);
   curpoint += segmentsize;
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
shringer EasyParser::absorbNumberStringPairShringer(){
   
   int segmentsize = 0;
   if(!absorbInt(segmentsize)){
      return shringer();
   }
   
   return absorbBuffer(segmentsize);
   
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser EasyParser::absorbNumberStringPairSubparser(){
   int length = 0;
   if(!absorbInt(length)){
      return EasyParser();
   }

   return absorbSubparser(length);
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser EasyParser::absorbSubparser(int64_t segmentsize){
   _unicode_disabled = true;
   
   if(curpoint + segmentsize > inputlength)
      return EasyParser();
   
   // let's see
   const char * movedforward = input + curpoint;
   EasyParser ret(movedforward, segmentsize);
   
   curpoint += segmentsize;
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbUnsignedLong(uint64_t& thelong){
   return absorbLong(*(int64_t*)&thelong);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbLong(int64_t& thelong){

   _unicode_disabled = true;

   if(curpoint + 8 > inputlength)
      return false;
      
   string thestr(input + curpoint, 8);
   
   thelong = bigendian? BigEndian::byteStringToLongLong(thestr) : Morestring::byteStringToLongLong(thestr);
   
   curpoint += 8;
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbDouble(double& thedouble){

   _unicode_disabled = true;

   if(curpoint + 8 > inputlength)
      return false;
   
   string thestr(input + curpoint, 8);
   
   uint64_t theint = bigendian ? BigEndian::byteStringToLongLong(thestr) : Morestring::byteStringToLongLong(thestr);
   thedouble = *(double*)&theint;
   
   curpoint += 8;
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbFloat(float& thefloat){

   _unicode_disabled = true;

   if(curpoint + 4 > inputlength)
      return false;
   
   string thestr(input + curpoint, 4);
   
   int theint = bigendian ? BigEndian::byteStringToInt(thestr) : Morestring::byteStringToInt(thestr);
   thefloat = *(float*)&theint;
   
   curpoint += 4;
   
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbString(int length, string& ret){

   _unicode_disabled = true;

   if(length < 0){
      return false;
   }
   
   if(curpoint + length > inputlength){
      return false;
   }
      
   ret.append(input + curpoint, length);
   curpoint += length;
   
   return true;

}
//////////////////////////////////////////////////////////////////////////////////////////
SDate EasyParser::parseDateSmartish(char day_or_month_first){
   
   bool assume_month_first = day_or_month_first == 'm';
   
   int year = -1;
   int month = -1;
   int monthday = -1;
   
   int weekday = -1;

   bool got_hour = false;   
   int hour = 0;
   int minutes = 0;
   int seconds = 0;
   
   bool iso8601_format = false;
   
   int floating_number = -1;
   bool date_complete = false;
   
   
   
   
   bool got_offset_spec = false;
   int hours_offset = 0;
   int minutes_offset = 0;
   
   
   EasyParser failer = *this;
   
   auto parsa = *this;
   
   while(parsa--.hasChar()){
      
      int nextchar = parsa.peekChar();
      if(isalpha(nextchar) && month == -1){
         string label = __toLower(parsa.parseLabelNoNum());
         eostri eoser = eostri::head(label);
         EOSTRIswitch(eoser){
         
         
            EOSTRIcase(jan)
            EOSTRIcase(january)
               month = 1;
               break;
            EOSTRIcase(feb)
            EOSTRIcase(february)
               month = 2;
               break;
            EOSTRIcase(mar)
            EOSTRIcase(march)
               month = 3;
               break;
            EOSTRIcase(apr)
            EOSTRIcase(april)
               month = 4;
               break;
            EOSTRIcase(may)
               month = 5;
               break;
            EOSTRIcase(jun)
            EOSTRIcase(june)
               month = 6;
               break;
            EOSTRIcase(jul)
            EOSTRIcase(july)
               month = 7;
               break;
            EOSTRIcase(aug)
            EOSTRIcase(august)
               month = 8;
               break;
            EOSTRIcase(sep)
            EOSTRIcase(september)
               month = 9;
               break;
            EOSTRIcase(oct)
            EOSTRIcase(october)
               month = 10;
               break;
            EOSTRIcase(nov)
            EOSTRIcase(november)
               month = 11;
               break;
            EOSTRIcase(dec)
            EOSTRIcase(december)
               month = 12;
               break;
            
            break;
            
         }
         
         if(month == -1){
            break;
         } else {
            *this = parsa;
            // cool, I guess
         }
      } else if (month != -1 && nextchar == 'T'){
      
         parsa.accept();
         *this = parsa;
         
         iso8601_format = true;
         // now parsing time
         
      } else if (isdigit(nextchar)){
         // 0-9
         string digstr = parsa.parseNumberString();
         if(digstr.size() == 8){
            
            string yearbit = digstr.substr(0, 4);
            string monthbit = digstr.substr(4, 2);
            string monthdaybit = digstr.substr(6, 2);
            year = Morestring::toInt(yearbit);
            month = Morestring::toInt(monthbit);
            monthday = Morestring::toInt(monthdaybit);
            *this = parsa; // cool I guess
            
         } else if(digstr.size() == 4){
            // it's year, bitches
            year = Morestring::toInt(digstr);
            *this = parsa;
         } else if(digstr.size() <= 2){
            int thenum = Morestring::toInt(digstr);
            
            if (parsa << "th" || parsa << "nd" || parsa << "rd" || parsa << "st"){
               // definitely date
               monthday = thenum;
               *this = parsa;
            } else if(parsa-- << ':'){
               
               hour = thenum;
               got_hour = true;
               minutes = parsa.parseNumber();
               if(parsa-- << ':'){
                  seconds = parsa.parseNumber();
               }
               
               parsa--;
               if(parsa << "am" || parsa << "AM"){
                  // just an hour
                  if(thenum == 12)
                     hour = 0;
               } else if (parsa <<"pm" || parsa << "PM"){
                  if(thenum == 12){
                     hour = 12; // grrr
                  }
               }
               
               *this = parsa;
               
               
            } else if (parsa << "am" || parsa << "AM"){
               // just an hour
               hour = thenum;
               got_hour = true;
               if(thenum == 12)
                  hour = 0;
               
               *this = parsa;
               
            } else if (parsa << "pm" || parsa << "PM"){
               hour = thenum + 12;
               got_hour = true;
               if(thenum == 12){
                  hour = 12; // grrr
               }
               
               *this = parsa;
               
            } else {
            
               if(monthday != -1 && month != -1){
                  break; // nope, something's wrong
               }
               
               // it's a floating number
               if(floating_number == -1){
                  floating_number = thenum;
               } else {
                  
                  if(assume_month_first || year != -1){
                     // or if year is there, then month comes after year
                     // goddamn americans
                     month = floating_number;
                     monthday = thenum;
                  } else {
                     monthday = floating_number;
                     month = thenum;
                  }
                  floating_number = -1;
                  
               }
               *this = parsa;
               
            
            }
            
         } else {
            break; // don't accept it, and finish
         }
      } else if((nextchar == 'Z') && got_hour && !got_offset_spec){
         // well
         
         got_offset_spec = true;
         parsa.accept();
         *this = parsa;
         
      } else if((nextchar == '-' || nextchar == '+') && got_hour && !got_offset_spec){
         
         
         bool negative = nextchar == '-';
         parsa.accept();
         
         
         string numstr = parsa.parseNumberString();
         if(numstr.size() == 4){
            hours_offset = Morestring::toInt(numstr.substr(0, 2));
            minutes_offset = Morestring::toInt(numstr.substr(2, 2));
         } else if (numstr.size() == 2){
            hours_offset = Morestring::toInt(numstr);
            
            if(parsa << ':'){
               minutes_offset = parsa.parseNumber();
               // cool enough
            }
            
         } else {
            break; // peepoweirdchamp
         }
         
         if(negative){
            hours_offset = -hours_offset;
            minutes_offset = -minutes_offset;
         }
         
         got_offset_spec = true;
         *this = parsa;
         
         
      } else if(nextchar == '-' && !iso8601_format){
         // just accept, for now
         // well this bit is fine
         parsa.accept(); // but don't put to *this
      } else if(nextchar == '/'){
         if(iso8601_format){
            break; // can't accept it post iso
         } else {
            parsa.accept(); // but don't put to *this either
         }
         
      } else {
         // weird char
         break;
      }
      
      // if complete, break
      if(year != -1 && month != -1 && monthday != -1 && got_hour && got_offset_spec){
         break;
      }
      
      
   }
   
   if(monthday == -1){
      monthday = floating_number;
   }
   
   if(year != -1 && month != -1 && monthday != -1){
      // we'll worry about the rest next time
      
      if(got_offset_spec){
         SDate ret = SDate::makeGlobalTime(monthday, month, year, hour, minutes, seconds, hours_offset, minutes_offset);
         ret.tv.tv_usec = 0;
         return ret;
      }
      
      SDate ret(monthday, month, year, hour, minutes, seconds);
      ret.tv.tv_usec = 0;
      return ret;
   } else {
      *this = failer;
      return SDate(0); // fail case
   }
   
}
//////////////////////////////////////////////////////////////////////////////////////////
// includes absorbing length as well
bool EasyParser::absorbNumberStringPair(string& ret){

   int length = 0;
   if(!absorbInt(length)){
      return false;
   }

   return absorbString(length, ret);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbBSONString(string& ret){
   int length = 0; // including the ender \0
   if(!absorbInt(length))
      return false;
   
   if(length <= 0)
      return false;
      // at least 1, because of the ender;
   
   if(!absorbString(length - 1, ret))
      return false;
   
   char ender = 0;
   if(!absorbByte(ender))
      return false;
   
   if(ender != 0)
      return false;
   // maybe use exception next time, but meh
   
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbBSONCstring(string& ret){
   while(true){
      char thechar;
      if(absorbChar(thechar)){
         if(thechar == 0)
            return true;
         ret.append(1, thechar);
      } else {
         return false;
      }
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbIntoMemory(char * ptr, int length){
   // to be templated
   _unicode_disabled = true;
   
   if(curpoint + length > inputlength)
      return false;

   memcpy(ptr, input + curpoint, length);
   curpoint += length;
   

   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::absorbIntoMembuf(int length, Shing::Membuf& buf){

   _unicode_disabled = true;
   
   if(length < 0)
      return false;
   
   if(curpoint + length > inputlength)
      return false;
      
   buf.copyfrom(input + curpoint, length);
   curpoint += length;

   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::getErrorLineMarkup(){
   int64_t moo;
   return getErrorLineMarkup(moo);
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::getErrorLineMarkup(int64_t& errorpoint){
   
   if(input != NULL){
      string readsofar(input, curpoint);
      string remainder(input + curpoint, inputlength - curpoint);
      
      errorpoint = curpoint;
      
      return "<245>" + Morestring::escapeForMarkup(readsofar) + "<88>" + Morestring::escapeForMarkup(remainder);
   } else {
      return "(error on stream reading)";
   }
   
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::throwError(const string& errmsg){

   if(input != NULL){
      string readsofar(input, curpoint);
      string remainder(input + curpoint, inputlength - curpoint);
      throw Serror(errmsg, (stroust[220] << "Error line : " >> 196 << readsofar >> 240 << remainder) <3);
   } else {
      throw Serror(errmsg, "no line info though");
   }

}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::error(const string& errmsg, bool assertquit, bool exitquit){

   oustoust[196] << "Error : " >> 245 << errmsg << endl;
   
   if(input != NULL){
      string readsofar(input, curpoint);
      string remainder(input + curpoint, inputlength - curpoint);
      oustoust[245] << readsofar >> 88 << remainder << endl;
      
      oustoust[240];
      int i;
      for(i = 0; i < curpoint; i++){
         oustoust << " ";
      }
      oustoust[196] << "^" << ozendl;
      
   } else {
   }
   if(assertquit)
      assert(0);
   
   if(exitquit){
      Shing::properMainthreadExit(1);
   }
      
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::erroust(const string& errmsg, bool assertquit, bool exitquit){

   ::erroust[196] << "Error : " >> 245 << errmsg << ozendl;
   
   if(input != NULL){
      string readsofar(input, curpoint);
      string remainder(input + curpoint, inputlength - curpoint);
      ::erroust[245] << readsofar >> 88 << remainder << ozendl;
      
      ::erroust[240];
      int i;
      for(i = 0; i < curpoint; i++){
         ::erroust << " ";
      }
      ::erroust[196] << "^" << ozendl;
   } else {
   }
   
   if(assertquit)
      assert(0);
   
   if(exitquit){
      Shing::properMainthreadExit(1);
   }
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::hasCharNoUnicode(int64_t numofchars){

   return curpoint + numofchars <= inputlength;

}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::hasChar(int64_t numofchars){
   if(!_unicode)
      return hasCharNoUnicode(numofchars);
   
   assert(input != NULL); // otherwise, but yeah
   
   int thepeek = peekChar(numofchars - 1);
   if(thepeek == EOF)
      return false;
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::getCharNoUnicode(){
   
   _unicode_disabled = true;
   
   int ret = 0;

   if(curpoint >= inputlength){
      ret = EOF;
   } else {
      uint8_t theret = input[curpoint];
      
      curpoint++;
      ret = theret;
   }
   
   if(ret == '\n'){
      linenum++;
      colnum = 0;
   } else {
      colnum++;
   }
   
   return ret;
   
   
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::getChar(){

   if(!_unicode)
      return getCharNoUnicode();
      
   asserts(!_unicode_disabled, "Unicode Mode Conflict Detected!");

   int ret = 0;
   
   if(curpoint >= inputlength){
      ret = EOF;
   } else {
      
      const char * ender = input + inputlength;
      const char * w = input + curpoint;
      try {
         ret = utf8::next_noassert(w, ender);
      } catch (...){
         ret = EOF;
      }
      
      
      curpoint += (w - (input + curpoint));
      if(unicode_lookbuffer.size() != 0){
         unicode_lookbuffer.pop_front();
      }
      
      if(ret != EOF){
         unicode_lastaccepted = ret;
         acceptcount_unicode++;
      }
      
   }

   if(ret == '\n'){
      linenum++;
      colnum = 0;
   } else {
      colnum++;
   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::takeHexes(int64_t maxsize){
   string ret;
   int i;
   for(i =0 ; i < maxsize; i++){
      if(hasChar()){
         int thechar = peekChar();
         if(isxdigit(thechar)){
            ret.append(1, thechar);
            accept();
         } else break;
      }
   }
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::takeStringNoUnicode(int64_t maxsize){
   string ret;
   int i;
   for(i =0 ; i < maxsize; i++){
      if(hasChar()){
         int thechar = getChar();
         ret.append(1, thechar);
      } else break;
   }
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::peekStringNoUnicode(int64_t maxsize){
   string ret;
   int i;
   for(i =0 ; i < maxsize; i++){
      int thechar = peekChar(i);
      if(thechar != EOF){
         ret.append(1, thechar);
      } else break;
   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::peekString(int64_t maxsize){

   if(!_unicode)
      return peekStringNoUnicode(maxsize);

   string ret;
   int i;
   for(i =0 ; i < maxsize; i++){
      int thechar = peekChar(i);
      if(thechar != EOF){
         Morestring::appendUtf8Char(ret, thechar);
      } else break;
   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::operator[](int64_t theindex){
   return peekChar(theindex, false);
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::peekChar(int64_t peekindex, bool nounicode){
   if(!_unicode || nounicode){
      return peekCharNoUnicode(peekindex);
   }
   
   asserts(!_unicode_disabled, "Unicode Mode Conflict Detected!");
   
   assert(input);
      
   if(unicode_lookbuffer.size() == 0){
      unicode_bufferendpoint = curpoint; // I guess, I suppose?
   }
   
   while(peekindex >= unicode_lookbuffer.size()){
      if(unicode_bufferendpoint >= inputlength){
         return EOF;
      }
      
      int toadd = 0;
      
      const char * ender = input + inputlength;
      const char * w = input + unicode_bufferendpoint;
      try {
         toadd = utf8::next_noassert(w, ender);
      } catch (...){
         toadd = EOF;
      }
      
      if(toadd == EOF)
         return EOF;
         
      unicode_bufferendpoint = w - input;
      unicode_lookbuffer.push_back(toadd);
      
   }
   
   return unicode_lookbuffer[peekindex]; // good, I guess?
   
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::peekCharNoUnicode(int64_t peekindex){

   _unicode_disabled = true;

   if(curpoint + peekindex >= inputlength){
      return EOF;
   }
   
   return (uint8_t)input[curpoint + peekindex];
}
//////////////////////////////////////////////////////////////////////////////////////////
int64_t EasyParser::segmentSize(){
   return inputlength;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::finished(){
   return !hasChar();
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::accept(){
   return getChar() != EOF;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::acceptWholeLabel(const shringer& thestring){
   EasyParser copia = *this;
   if(!(copia << thestring))
      return false;
   // accepted, so now.
   if(copia.peekingAtLabelStart())
      return false;
   
   *this = copia;
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::accept(int thechar){
   return acceptChar(thechar);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::acceptNChars(int then, bool nounicode){

   if(then == 0)
      return true; // beh

   if(peekChar(then - 1, nounicode) == EOF)
      return false;
   
   int i;
   for(i = 0; i < then; i++){
      accept();
   }
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::allWhitesSinceBeginningOfLine(){
   assert(input != NULL);
   
   int64_t che = curpoint - 1;
   // should be good for utf8
   
   while(che >= 0){
      if(input[che] == '\n')
         return true; // hit the line beginning!
      
      if(!iswspace(input[che]))
         return false;
      
      che--;
   }
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::disableUnicode(bool truth){
   _unicode = !truth;
   return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::unicodeIsDisabled(){
   return !_unicode || _unicode_disabled;
}
//////////////////////////////////////////////////////////////////////////////////////////
int64_t EasyParser::currentPoint(){
   return curpoint;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::accept(const shringer& theseq, bool nounicode){
   return acceptSeq(theseq, nounicode);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::acceptSeqCasedNoUnicode(const shringer& theseq, bool caselessness){
   int i;
   if(caselessness){
      for(i = 0; i < theseq.size(); i++){
         if(tolower(peekChar(i)) != tolower(theseq.getCharAt(i)))
            return false;
      }
   } else {
      for(i = 0; i < theseq.size(); i++){
         if(peekChar(i) != theseq.getCharAt(i))
            return false;
      }
   }
      
   for(i = 0; i < theseq.size(); i++){
      accept();
   }
   
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::acceptSeqCased(const shringer& theseq, bool caselessness, bool nounicode){

   if(!_unicode || nounicode){
      return acceptSeqCasedNoUnicode(theseq, caselessness);
   }
   
   UTF8EasyParser subparsa(theseq);
   // unicode is active by default!
   int64_t charcount = 0;
   while(true){
      int lechar = subparsa.getChar();
      
      if(lechar == EOF)
         break;
      
      if(caselessness){
         if(towlower(lechar) != towlower(peekChar(charcount)))
            return false;
      } else {
         if(lechar != peekChar(charcount))
            return false;
      }
      
      charcount++;
   }
   
   int i;
   for(i = 0; i < charcount; i++){
      bool succ = accept();
      assert(succ);
   }
   
   return true;
   

}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::acceptSeq(const shringer& theseq, bool nounicode){
   return acceptSeqCased(theseq, _caseless, nounicode);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::acceptCharNoUnicode(int expected){
   return acceptChar(expected, true);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::acceptChar(int expected, bool nounicode){
   int thechar = nounicode ? peekCharNoUnicode() : peekChar();
   if(_caseless){
      if(towlower(thechar) == towlower(expected)){
         accept();
         return true;
      }
   } else {
      if(thechar == expected){
         accept();
         return true;
      }
   }
   return false;
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::mustAcceptChar(char thechar, bool nounicode){
   if(!acceptChar(thechar, nounicode))
      error("Expecting : " + __toString(thechar));
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::isPeekingAtNoUnicodeCased(const shringer& theseq, bool caselessness){
   int i;
   if(caselessness){
      for(i = 0; i < theseq.size(); i++){
         if(tolower(peekChar(i)) != tolower(theseq.getCharAt(i)))
            return false;
      }
   } else {
      for(i = 0; i < theseq.size(); i++){
         if(peekChar(i) != theseq.getCharAt(i))
            return false;
      }
   }
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::isPeekingAt(const shringer& theseq, bool nounicode){
   return isPeekingAtCased(theseq, _caseless, nounicode);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::isPeekingAtCased(const shringer& theseq, bool caselessness, bool nounicode){
   
   if(!_unicode){
      return isPeekingAtNoUnicodeCased(theseq, caselessness);
   }
   
   EasyParser subparsa(theseq);
   // unicode is active by default!
   int64_t charcount = 0;
   while(true){
      int lechar = subparsa.getChar();
      
      if(lechar == EOF)
         break;
      
      if(caselessness){
         if(towlower(lechar) != towlower(peekChar(charcount)))
            return false;
      } else {
         if(lechar != peekChar(charcount))
            return false;
      }
      
      charcount++;
   }
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtOctal(){
   int thechar = peekChar();
   return thechar >= '0' && thechar < '8';
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::parseOctal(int maxlen){
   string ret;
   while(true){
   
      if(maxlen != 0 && ret.size() >= maxlen)
         break;
   
      int curchar = peekChar();
      
      if(curchar >= '0' && curchar < '8'){
         ret.append(1, curchar);
         accept();
      } else {
         break;
      }
   }
   
   return strtol(ret.c_str(), NULL, 8);
   
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtHex(){
   int thechar = towlower(peekChar());
   return (thechar >= '0' && thechar <= '9') || (thechar >= 'a' && thechar <= 'f');
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtColorCode(){
   return operator==('#') || peekingAtNumbers();
}
//////////////////////////////////////////////////////////////////////////////////////////
shingcolor EasyParser::parseColorCode(){
   if(peekingAtNumbers()){
      
      // well is it a float?
      auto iof = parseIntOrFloat();
      if(iof.isint){
         int colorcode = iof.intret;
         operator<<("_xcoel"); // optional but whatever
         
         shingcolor ret = shingcolor::getXTermColor(colorcode);
         if(operator<<('%')){
            float alphaval = parseFloat();
            ret.setAlpha(alphaval * ret.getAlpha());
         }
         return ret;
      } else {
         
         return shingcolor(iof.floatret); // a gray value
         
      }
   } else if (operator<<('#')){
      return parseColor();
   }
   
   return shingcolor(); // beh
}
//////////////////////////////////////////////////////////////////////////////////////////
ColorInfo EasyParser::parseColorInfo(){
   if(peekingAtNumbers()){
      
      // well is it a float?
      auto iof = parseIntOrFloat();
      if(iof.isint){
         int colorcode = iof.intret;
         operator<<("_xcoel"); // optional but whatever
         
         ColorInfo ret = shingcolor::getXTermColor(colorcode);
         sw();
         if(operator<<('%')){
            sw();
            float alphaval = parseFloat();
            ret.setAlpha(alphaval * ret.getAlpha());
         }
         return ret;
      } else {
         
         ColorInfo ret(iof.floatret); // a gray value
         
         sw();
         if(operator<<('%')){
            sw();
            float alphaval = parseFloat();
            ret.setAlpha(alphaval * ret.getAlpha());
         }
         return ret;
         
      }
   } else if (operator<<('#')){
      return parseColor();
   }
   
   return ColorInfo(); // beh
}
//////////////////////////////////////////////////////////////////////////////////////////
shingcolor EasyParser::parseColor(){
   string hexstr = parseHexString(8); // maximum 8 right?
   return shingcolor(hexstr);
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseIPAddress(){
   EasyParser tempa = *this;
   
   string ret;
   int i;
   for(i = 0; i < 4; i++){
      if(!tempa.peekingAtNumbers())
         return ""; // failed
         
      int numa = tempa.parseNumber();
      if(numa >= 0 && numa <= 255){
         ret.append(__toString(numa));
      } else {
         return ""; // bad IP address, I guess
      }
      
      
      if(i == 3)
         break; // done
      
      if(tempa << '.'){
         // cool
         ret.append(1, '.');
      } else {
         return "";
      }
      
   }
   
   // cool
   
   *this = tempa;
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::parseHex(int maxlen){
   string ret = parseHexString(maxlen);
   return strtol(ret.c_str(), NULL, 16);
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseHexString(int maxlen){
   string ret;
   while(true){
   
      if(maxlen != 0 && ret.size() >= maxlen)
         break;
   
      int curchar = towlower(peekChar());
      
      if((curchar >= '0' && curchar <= '9') || (curchar >= 'a' && curchar <= 'f')){
         ret.append(1, curchar);
         accept();
      } else {
         break;
      }
   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool easyparsa_whitespace_or_alphanum(char lechar){
   return iswspace(lechar) || isalnum(lechar);
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseSomeSentenceElement(){
   string ret = parseLabel();
   if(ret != ""){
      return ret;
   }
   
   ret = parseNumberString();
   if(ret != "")
      return ret + parseLabel(); // yep
   
   return parseUntil(easyparsa_whitespace_or_alphanum, false);
   
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
Shing::AnimRange EasyParser::parseAnimRange(){
   // starts with a label but yeah
   auto& selfie = *this;
   
   if(!selfie.paLS()){
      return Shing::AnimRange();
   }
   
   
   Shing::AnimRange ret;
   ret.name = selfie.parseFileName();
   
   // fras_step_f.f (-0.85) 0.5x
   // fras_step_f (0.2-) 1.2x
   
   EasyParser tempa = selfie;
   tempa--;
   if(tempa << '('){
      flxoat firstflxoat;
      static const flxoat percmult = 10_flxm;
      
      if(tempa--.paNUM()){
         firstflxoat = tempa.parseFlxoat() * percmult;
      }
      
      if(tempa << '-'){
         // well
         
         flxoat secondflxoat = 1_flx;
         if(tempa--.paNUM()){
            secondflxoat = tempa.parseFlxoat() * percmult;
         }
         
         
         // well, let's go
         
         if(tempa-- << ')'){
            selfie = tempa; // accept the bracketing
            ret.zto_start = firstflxoat;
            ret.zto_end = secondflxoat;
         }
         
         
         
      }
      
   }
   
   tempa = selfie;
   if(tempa--.paNUM()){
      flxoat numero = tempa.parseFlxoat();
      if(tempa << 'x'){
         ret.speed = numero;
         ret.dur = -1_flx;
         selfie = tempa;
      } else if (tempa << 'f'){
         ret.speed = -1_flx;
         ret.dur = numero;
         // duration is in frames of course
         selfie = tempa;
      }
      
   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtPhoneNumber(){
   if((*this) == '+'){
      return isdigit(peekChar(1));
   }
   return isdigit(peekChar());
}
//////////////////////////////////////////////////////////////////////////////////////////
Shing::PhoneNumber EasyParser::parsePhoneNumber(){
   Shing::PhoneNumber ret;
   
   if(!peekingAtPhoneNumber())
      return ret; // not a number
   
   int64_t startpoint = currentPoint();
   ret.has_plus = operator<<('+');
   
   auto& parsa = *this;
   
   while(isdigit(parsa.peekChar())){
      
      int thenum = parsa.getChar();
      ret.numbers.append(1, thenum);
      
      EasyParser tempa = parsa;
      if(tempa << ' ' || tempa == '-'){
         if(isdigit(tempa.peekChar())){
            parsa = tempa;
            // continue
         } else {
            break; // we finished here
         }
      }
      
   }
   
   int64_t endpoint = currentPoint();
   ret.raw_text = getShringerFromTo(startpoint, endpoint).toString();
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::parseNumberMaybeNegative(){
   int mult = 1;
   if(peekChar() == '-'){
      mult = -1;
      accept();
   }
   return mult * parseNumber();
}
//////////////////////////////////////////////////////////////////////////////////////////
int64_t EasyParser::parseCommaedNumber(){
   string numberstr = parseNumberString();
   
   while(peekChar(0) == ',' && isdigit(peekChar(1))){
      (*this) << ',';
      numberstr.append(parseNumberString());
      // cool
   }
   
   return Morestring::toInt(numberstr);
}
//////////////////////////////////////////////////////////////////////////////////////////
int64_t EasyParser::parseNumber(){
   string ret;
   while(true){
      int curchar = peekChar();
      
      if(isdigit(curchar)){
         ret.append(1, curchar);
         accept();
      } else {
         break;
      }
   }
   
   return atoi(ret.c_str());
   
}
//////////////////////////////////////////////////////////////////////////////////////////
Shing::flxoat EasyParser::parseFlxoat(){
   
   // this is compliocated
   
   bool negativit = (*this) << '-';
   
   int numbit = parseNumber();
   
   if((*this) << '.'){
      
      string secondnumberbit = parseNumberString();
      // herehere
      int secondnumber = secondnumberbit.size() == 0 ? 0 : atoi(secondnumberbit.c_str());
      
      int charcount = secondnumberbit.size();
      switch(secondnumberbit.size()){
         case 0:
            // nothing
            return flxoat::fromint(numbit).maybeNegate(negativit);
         case 1:
            return flxoat::fromIntAndMilpart(numbit, secondnumber * 100000).maybeNegate(negativit);
         case 2:
            return flxoat::fromIntAndMilpart(numbit, secondnumber * 10000).maybeNegate(negativit);
         case 3:
            return flxoat::fromIntAndMilpart(numbit, secondnumber * 1000).maybeNegate(negativit);
         case 4:
            return flxoat::fromIntAndMilpart(numbit, secondnumber * 100).maybeNegate(negativit);
         case 5:
            return flxoat::fromIntAndMilpart(numbit, secondnumber * 10).maybeNegate(negativit);
         case 6:
            return flxoat::fromIntAndMilpart(numbit, secondnumber * 1).maybeNegate(negativit);
         default:
            {
               string firstparter = secondnumberbit.substr(0, 6);
               int numbered = atoi(firstparter.c_str());
               return flxoat::fromIntAndMilpart(numbit, numbered * 1).maybeNegate(negativit);
            }
            // we clamp up, I guess
      }
      
   }
   
   return flxoat::fromint(numbit).maybeNegate(negativit);
   
}
//////////////////////////////////////////////////////////////////////////////////////////
float EasyParser::parseFloat(){
   return parseIntOrFloatMaybeNegative().asfloat();
}
//////////////////////////////////////////////////////////////////////////////////////////
IntOrFloat EasyParser::parseIntOrFloatMaybeNegative(){
   int mult = 1;
   if(peekChar() == '-'){
      mult = -1;
      accept();
   }
   IntOrFloat ret = parseIntOrFloat();
   ret.intret *= mult;
   ret.floatret *= mult;
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
IntOrFloat EasyParser::parseIntOrFloat(){

   string ret;
   bool gotdot = false;
   bool gotexponent = false;
   
   while(true){
      int curchar = peekChar();
      
      if(isdigit(curchar)){
         ret.append(1, curchar);
         accept();
      } else if (!gotexponent && curchar == '.' && !gotdot){
         gotdot = true;
         ret.append(1, curchar);
         accept();
      } else if (!gotexponent && (curchar == 'e' || curchar == 'E')){
         
         int theafter = peekChar(1);
         if(isdigit(theafter)){
            ret.append(1, curchar);
            ret.append(1, theafter);
            accept();
            accept();
            gotexponent = true;
         } else if(theafter == '-' && isdigit(peekChar(2))){
            accept();
            accept();
            ret.append(1, curchar);
            ret.append(1, theafter);
            gotexponent = true;
         } else {
            break;
         }
         
      } else {
         break;
      }
   }
   
   if(gotdot){
      return IntOrFloat((float)atof(ret.c_str()));
   } else {
      return IntOrFloat(atoi(ret.c_str()));
   }
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseIntOrFloatString(bool& isfloat){
   string ret;
   bool gotdot = false;
   
   while(true){
      int curchar = peekChar();
      
      if(isdigit(curchar)){
         ret.append(1, curchar);
         accept();
      } else if (curchar == '.' && !gotdot){
         gotdot = true;
         ret.append(1, curchar);
         accept();
      } else {
         break;
      }
   }
   
   isfloat = gotdot;
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseDottedNumberString(){
   string ret = parseNumberString();
   if(ret != ""){
      while(peekChar() == '.' && isdigit(peekChar(1))){
         ret.append(1, '.');
         accept();
         
         ret.append(parseNumberString()); // yep
         
      }
   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseNumberString(bool maybeneg){
   string ret;
   
   if(maybeneg && peekChar() == '-'){
      ret.append(1, '-');
      accept(); // why not
   }

   while(true){
      int curchar = peekChar();
      
      if(isdigit(curchar)){
         ret.append(1, curchar);
         accept();
      } else {
         break;
      }
   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::expectLabels(){
   if(!peekingAtLabelStart())
      error("expecting labels here!");
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtLabelStart(){
   int thechar = peekChar();
   return (iswalpha(thechar) || thechar == '_');
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtAlnum(){
   int peeka = peekChar();
   return iswalpha(peeka) || isdigit(peeka);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtAlnumOrUnderscore(){
   int peeka = peekChar();
   return iswalpha(peeka) || isdigit(peeka) || peeka == '_';
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtAlphabet(){
   return iswalpha(peekChar());
}
//////////////////////////////////////////////////////////////////////////////////////////
int romanNumeralCharValue(int thechar, bool allownocaps){
   switch(thechar){
      case 'I': return 1;
      case 'V': return 5;
      case 'X': return 10;
      case 'L': return 50;
      case 'C': return 100;
      case 'D': return 500;
      case 'M': return 1000;
   }
   
   return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::parseRomanEx(string& ret, int& thenumber, bool allownocaps){
   
   thenumber = 0;
   ret = "";
   
   while(hasChar()){
      
      char thechar = peekChar();
      
      int curval = romanNumeralCharValue(thechar, allownocaps);
      
      if(curval == 0){
         break;
      }
      
      accept();
      ret.append(1, thechar);
      
      int valuetoadd = curval;
      
      if(hasChar() && (curval == 1 || curval == 10 || curval == 100)){
         
         char nextchar = peekChar();
         int nextval = romanNumeralCharValue(nextchar, allownocaps);
         if(nextval == curval * 10 || nextval == curval * 5){
            
            ret.append(1, nextchar);
            accept();
            
            valuetoadd = nextval - curval;
         }
         
      }
      
      thenumber += valuetoadd;
      
   }
   // cool
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseRomanNumeralString(bool allownocaps){
   string ret;
   int thenum;
   parseRomanEx(ret, thenum, allownocaps);
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::parseRomanNumeral(bool allownocaps){
   string ret;
   int thenum;
   parseRomanEx(ret, thenum, allownocaps);
   return thenum;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtRoman(bool allowlowcaps){
   int curchar = peekChar();
   if(allowlowcaps)
      curchar = towupper(curchar);
   
   return romanNumeralCharValue(curchar, allowlowcaps) != 0;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtAnyNumber(){
   return peekingAtNumbersMaybeNegative()
        || peekingAtOctal()
        || peekingAtHex()
        || peekingAtRoman(true);
}
//////////////////////////////////////////////////////////////////////////////////////////
int64_t EasyParser::parseAnyNumber(){
   if(peekingAtNumbersMaybeNegative())
      return parseNumberMaybeNegative();
   
   if(peekingAtOctal())
      return parseOctal();
   
   if(peekingAtHex())
      return parseHex();
   
   if(peekingAtRoman(true))
      return parseRomanNumeral(true);
   
   return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseAnyNumberString(){
   int64_t origpos = currentPoint();
   int ret = parseAnyNumber();
   
   return getShringerFrom(origpos).toString();
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::unicodeAppendStr(string& toappend, int thechar){
   if(_unicode){
      Morestring::appendUtf8Char(toappend, thechar);
   } else {
      toappend.append(1, thechar);
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::peekLabel(){
   string ret;
   int i = 0;
   for(i = 0; i < 100000; i++){
      // 100000 char max..
      int thechar = peekChar(i);
      if(iswalpha(thechar) || thechar == '_' || (i != 0 && isdigit(thechar))){
         unicodeAppendStr(ret, thechar);
      } else {
         break;
      }
   }
   
   return ret; // yeah
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::peekLabelNoNum(){
   string ret;
   int i = 0;
   for(i = 0; i < 100000; i++){
      // 100000 char max..
      int thechar = peekChar(i);
      if(iswalpha(thechar) || thechar == '_'){
         unicodeAppendStr(ret, thechar);
      } else {
         break;
      }
   }
   
   return ret; // yeah
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseNChars(int nchars){
   
   string ret;
   
   int i;
   for(i = 0; i < nchars; i++){
      int thechar = getChar();
      if(thechar == EOF){
         break;
      }
      unicodeAppendStr(ret, thechar);
   }
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseNCharsOrUntil(int nchars, int untiler, bool inclusive){
   
   string ret;
   
   int i;
   for(i = 0; i < nchars; i++){
      int thechar = peekChar();
      if(thechar == EOF){
         break;
      }
      
      if(thechar == untiler){
         if(inclusive){
            accept();
            unicodeAppendStr(ret, thechar);
         }
         break;
      }
      
      accept();
      unicodeAppendStr(ret, thechar);
   }
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
shringer EasyParser::parseB64(){
   shringer leftoverbuffer = getLeftoverBuffer();
   const char* cstr = leftoverbuffer.c_str();
   int64_t sizer = leftoverbuffer.size();
   
   
   bool got_padding = false;
   int i;
   for(i = 0; i < sizer; i++){
      char thechar = cstr[i];
      if(!got_padding && (isalnum(thechar) || thechar == '+' || thechar == '/')){
         // cool, I guess
      } else if (thechar == '='){
         // padding
         got_padding = true;
         // cool, can still pad
      } else {
         break; // it's over
      }
   }
   
   acceptNChars(i);
   return leftoverbuffer.substr(0, i);
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseLabelNoNum(){
   string ret;
   
   bool foundsth = false;
   
   while(true){
      int thechar = peekChar();
      
      if(iswalpha(thechar) || thechar == '_'){
         unicodeAppendStr(ret, thechar);
         accept();
      } else {
         break;
      }
      
      foundsth = true;

   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseAlphaNumericWord(){
   string ret;
   
   bool foundsth = false;
   
   while(true){
      int thechar = peekChar();
      
      if(isdigit(thechar) || iswalpha(thechar) || easyparser_is_apostrophe(thechar) || thechar == '_'){
         unicodeAppendStr(ret, thechar);
         accept();
      } else {
         break;
      }
      
      foundsth = true;

   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseNameWord(){
   string ret;
   
   bool foundsth = false;
   
   while(true){
      int thechar = peekChar();
      
      if(iswalpha(thechar) || easyparser_is_apostrophe(thechar) || thechar == '_'){
         unicodeAppendStr(ret, thechar);
         accept();
      } else {
         break;
      }
      
      foundsth = true;

   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseEnglishElement(char connector1, char connector2, char connector3){
   string ret;
   
   while(peekingAtAlnumOrUnderscore()){
      
      ret.append(parseAlphaNumericWord());
      
      int nextchar = peekChar();
      if(nextchar == connector1 || nextchar == connector2 || nextchar == connector3){
         
         int theoneafter = peekChar(1);
         if(iswalpha(theoneafter) || isdigit(theoneafter)){
            // well, good. accept. it, for now
            
            ret.append(1, nextchar);
            accept(); // why not
            
         } else {
            break; // don't.
         }
         
      } else {
         break;
      }
      
   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCSSClassName(){
   string ret;
   
   while(peekingAtAlnumOrUnderscore()){
      
      ret.append(parseAlphaNumericWord());
      
      int nextchar = peekChar();
      if(nextchar == '-'){
         
         int peekcount = 1;
         while(peekChar(peekcount) == '-'){
            peekcount++;
         }
         
         int theoneafter = peekChar(peekcount);
         if(iswalpha(theoneafter) || isdigit(theoneafter)){
            // well, good. accept. it, for now
            
            ret.append(peekcount, '-');
            int i;
            for(i = 0; i < peekcount; i++){
               accept(); // why not
            }
            
         } else {
            break; // don't.
         }
         
      } else {
         break;
      }
      
   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseAlphaNumericEnglishElement(){
   return parseEnglishElement('.', '-');
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseSpacedEnglishWordsAndNumbers(bool allow_numbers){
   int64_t currentstart = currentPoint();
   auto& parsa = *this;
   
   string ret = parseAlphaNumericEnglishElement();
   if(ret.size() == 0){
      return "";
   }
   
   int64_t endpoint = parsa.currentPoint();
   while(true){
      EasyParser tempa = parsa;
      
      if(tempa--.peekingAtLabelStart()){
         parsa = tempa;
         parsa.parseAlphaNumericEnglishElement();
         endpoint = parsa.currentPoint();
      } else {
         break;
      }
   }
   
   return getShringerFromTo(currentstart, endpoint).toString();
   /*
   
   if(ret == "")
      return ret;
      
   EasyParser tempa = *this;
   while(tempa--.hasChar()){
      string moreengword = tempa.parseEnglishWord();
      
      if(moreengword != ""){
         ret.append(1, ' ');
         ret.append(moreengword);
         *this = tempa;
      } else if (allow_numbers && tempa.peekingAtNumbers()){
         string numstr = tempa.parseNumberString();
         ret.append(1, ' ');
         ret.append(numstr);
         *this = tempa;
      } else {
         break;
      }
      
   }
   
   return ret;
   */
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseSpacedEnglishWords(){

   return parseSpacedEnglishWordsAndNumbers(false);
   
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseEnglishWord(){
   
   string ret;
   
   while(peekingAtAlphabet()){
      ret.append(parseNameWord());
      
      int nextchar = peekChar();
      if(nextchar == '.' || nextchar == '-'){
         
         int theoneafter = peekChar(1);
         if(iswalpha(theoneafter)){
            // well, good. accept. it, for now
            
            ret.append(1, nextchar);
            accept(); // why not
            
         } else {
            break; // don't.
         }
         
      } else {
         break;
      }
      
   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseXMLAttribLabel(){
   string ret;
   
   bool foundsth = false;
   
   while(true){
      int thechar = peekChar();
      
      if(iswalpha(thechar) || thechar == '_' || (foundsth && (isdigit(thechar) || thechar == ':' || thechar == '-'))){
         unicodeAppendStr(ret, thechar);
         accept();
      } else {
         break;
      }
      
      foundsth = true;

   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseLabel(){
   string ret;
   
   bool foundsth = false;
   
   while(true){
      int thechar = peekChar();
      
      if(iswalpha(thechar) || thechar == '_' || (foundsth && isdigit(thechar))){
         unicodeAppendStr(ret, thechar);
         accept();
      } else {
         break;
      }
      
      foundsth = true;

   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::parseCommaSet(function<void(const string& thingo)> lefun){
   
   auto& parsa = *this;
   
   while(parsa--.hasChar()){
      
      int nextchar = parsa.peekChar();
      if(nextchar == ']' || nextchar == ')')
         break;
      
      string thingo = __chompends(parsa.parseCPlusUntilBracketEnd([](int thechar, EasyParser& parsa){
         if(thechar == ',')
            return true;
         return false;
      }));
      
      if(thingo != ""){
         lefun(thingo);
      }
      
      if(parsa-- << ','){
         // continue
      } else {
         break;
      }
   }
   
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::parseCommaDict(function<void(const string& var, const string& val)> lefun){
   
   auto& parsa = *this;
   
   while(parsa--.hasChar()){
      
      int nextchar = parsa.peekChar();
      if(nextchar == ']' || nextchar == ')')
         break;
      
      string firstbit;
      while(parsa.hasChar()){
         if(parsa.peekingAtLabelStart() || parsa.peekingAtNumbers()){
            firstbit.append(parsa.parseAlphaNumericEnglishElement());
            break;
         } else {
            int thechar = parsa.peekChar();
            if(thechar == ',' || nextchar == ']' || nextchar == ')'){
               break;
            }
            
            firstbit.append(1, thechar);
            parsa.accept();
         }
      }
      
      if(firstbit != ""){
         
         if(parsa-- << ':' || parsa << '='){
            // cool I guess, accep this bit
            parsa--;
         }
         
         string valer = __chompends(parsa.parseCPlusUntilBracketEnd([](int thechar, EasyParser& parsa){
            if(thechar == ',')
               return true;
            return false;
         }));
         
         lefun(firstbit, valer);
         
      }
      
      if(parsa-- << ','){
         // continue
      } else {
         break;
      }
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
map<string, string> EasyParser::parseCommaDict(){
   map<string, string> ret;
   parseCommaDict([&ret](const string& a, const string& b){
      ret[a] = b;
   });
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseLispSymbol(){
   string ret;
   while(true){
      int thepeek = peekChar();
      
      if(thepeek == EOF)
         return ret;
      if(iswspace(thepeek))
         return ret;
      if(thepeek == ')' || thepeek == ';')
         return ret;
      
      // otherwise it should be good
      unicodeAppendStr(ret, thepeek);
      accept();
      
   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseStringLiteralSmartReturnRaw(){
   
   int64_t curpoint = currentPoint();
   parseStringLiteralSmart();
   
   int64_t newcurpoint = currentPoint();
   return getShringerFromTo(curpoint, newcurpoint).toString();
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseStringLiteralSmart(){
   if(peekChar() == '\''){
      return parseStringLiteral('\'');
   }
   
   return parseStringLiteral('"');
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseStringLiteral(int thesep){
   
   string ret;
   
   
   bool isinquotes = false;
   bool escaped = false;
   bool foundsth = false;
      
   
   while(true){
      int thechar = peekChar();
      
      if(thechar == EOF){
         break; // just hard break
      } else if(isinquotes){
         
         bool alreadyaccepted = false;
         
         if(thechar == thesep){
            if(escaped){
               unicodeAppendStr(ret, thesep);
               escaped = false;
            } else {
               isinquotes = false;
            }
         } else if (thechar == '\\'){
            if(escaped){
               ret.append(1, '\\');
               escaped = false;
            } else {
               escaped = true;
               
               accept();
               alreadyaccepted = true;
               
               // you know what, let's just see if we can pars enumber
               int nextpeek = peekChar();
               if(isdigit(nextpeek)){
                  int thenumber = parseNumber();
                  Morestring::appendUtf8Char(ret, thenumber);
                  escaped = false;
               }
               
            }
         } else {
            if(escaped){
               switch(thechar){
                  case 'u':
                     {
                        string fourhex = takeHexes(4);
                        uint32_t dehexed = Morestring::dehexifyint(fourhex);
                        ret.append(Morestring::unicodify(dehexed));
                     }
                     break;
                  case 'n':
                     ret.append(1, '\n');
                     break;
                  case 'r':
                     ret.append(1, '\r');
                     break;
                  default:
                     ret.append(1, '\\');
                     unicodeAppendStr(ret, thechar);
                     break;
               }
               
               escaped = false;
            } else {
               unicodeAppendStr(ret, thechar);
            }
         }
         
         if(!alreadyaccepted)
            accept();
         
      } else {
         if(thechar == thesep){
            isinquotes = true;
            accept();
         } else if(iswalpha(thechar) || thechar == '_' || (foundsth && isdigit(thechar))){
            unicodeAppendStr(ret, thechar);
            accept();
         } else {
            break;
         }
      }
      
      foundsth = true;

   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseJSRegex(bool untileol){
   
   bool withopener = false;
   vector<char> blocks; // active blocks
   
   if(accept('/'))
      withopener = true;
      
   bool escaped = false;
   
   string ret;
   
   while(hasChar()){
      int thechar = peekChar();
      
      if(thechar == '\\'){
         escaped = true;
         accept();
         unicodeAppendStr(ret, thechar);
      } else if (thechar == '/'){
         if(escaped){
            accept();
            unicodeAppendStr(ret, thechar);
            escaped = false;
         } else {
            if(withopener){
               accept();
            }
            break;
         }
      } else if(thechar == '\n' && untileol){
         break; // nope
      } else {
         accept();
         unicodeAppendStr(ret, thechar);
         escaped = false;
      }
   
   }
   
   return ret;
   
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string __parseUnixishStuff(EasyParser& parsa, bool allowspaces, bool allowat){
   string ret;
   bool finished = false;
   while(true){
      int thechar = parsa.peekChar();
      switch(thechar){
         case EOF:
         case '\\':
         case '?':
         case '%':
         case '*':
         case ':':
         case '|':
         case '"':
         case ',':
         case '<':
         case '>':
         case '(':
         case ')':
         case '[':
         case ']':
         case '{':
         case '}':
            finished = true;
            break; // well, invalid characters
         case '@':
            if(allowat){
               ret.append(1, thechar);
               parsa.accept();
            } else {
               finished = true;
            }
            break;
         case ' ':
         case '\t':
            if(allowspaces){
            } else {
               finished = true;
               break;
            }
         default:
            parsa.unicodeAppendStr(ret, thechar);
            parsa.accept();
      }
      
      if(finished)
         break;

   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseSentenceElement(){
   
   skipWhitespacesExceptNewline();
   
   if(!hasChar())
      return "";
   
   int curchar = peekChar();
   if(peekingAtAlnum()){
      return parseAlphaNumericEnglishElement();
      
      // well, yeah. return a sentence element, or something
   } else {
      
      // there's gotta be something, including newlines right?
      accept();
      return Morestring::unicodify(curchar);
      // but it could be a 4chan!
      
   }
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseURLElement(bool allowspaces){

   EasyParser temp = *this;
   
   if(temp | "(0)://"){
      string thefront = temp(0);
      if(thefront != ""){
         string ret = thefront + "://";
         
         while(temp.hasChar()){
            string theadd = __parseUnixishStuff(temp, allowspaces, false);
            if(theadd == "")
               break;
            
            ret.append(theadd);
            
            if(temp << '/'){
               // can continue
               ret.append(1, '/');
            } else {
               break;
            }
            
         }
         *this = temp;
         return ret;
      }
   }
   
   string ret = __parseUnixishStuff(*this, allowspaces, false);
   while(accept('/')){
      ret.append(1, '/');
      ret.append(__parseUnixishStuff(*this, allowspaces, false));
   }
   
   return ret;
   
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseFileName(){
   return __parseUnixishStuff(*this, false, true);
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseUnixFileName(bool allowspaces){
   return __parseUnixishStuff(*this, allowspaces, true);
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseUntilEOL(){
   string ret;
   
   while(true){
      int thechar = peekChar();

      if(thechar == EOF || thechar == '\n'){
         break;
      } else {
         unicodeAppendStr(ret, thechar);
         accept();
      }

   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseUntilEnd(){

   string ret;
   
   while(true){
      int thechar = peekChar();

      if(thechar == EOF){
         break;
      } else {
         unicodeAppendStr(ret, thechar);
         accept();
      }

   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseLine(){
   string ret = parseUntil('\n', false);
   accept('\n');
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseUntilString(const shringer& towatch, bool inclusive){

   string ret;

   while(true){
      int thechar = peekChar();
      
      if(thechar == EOF){
         break;
      } else if(!isPeekingAt(towatch)){
         unicodeAppendStr(ret, thechar);
         accept();
      } else {
         // well, its' peeking at it
         // thechar == limiter
         if(inclusive){
            ret.append(towatch.c_str(), towatch.size());
            acceptSeq(towatch);
         }
         break;
      }

   }
   
   return ret;

}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::processBracketContents(char brackettype, char equaler, char delimiter, function<void(const string& dom, const shringer& val)> func){
   auto& parsa = *this;
   if(parsa << brackettype){
      // accepted
   } else {
      return false;
   }
   
   gplaicht(hyperplate_mode);
   
   char bracketcloser = ')';
   if(brackettype == '[')
      bracketcloser = ']';
   if(brackettype == '<')
      bracketcloser = '>';
   if(brackettype == '{')
      bracketcloser = '}';
   
   while(parsa.hasChar()){
      
      if(parsa << bracketcloser){
         break; // finished
      }
      
      parsa--;
      
      string attribname;
      int spacecount = 0;
      while(parsa.hasChar()){
         int thepeek = parsa.peekChar();
         if(thepeek == equaler || thepeek == bracketcloser || thepeek == delimiter){
            break;
         }
         
         if(hyperplate_mode && thepeek == ' ')
            break; // spaces count, as well
         
         parsa.accept();
         
         if(thepeek == ' '){
            spacecount++;
         } else {
            attribname.append(spacecount, ' ');
            spacecount = 0;
            attribname.append(1, thepeek); // yeah, simple as that
         }
         
      }
      
      
      if(attribname == "")
         break; // bad format, I guess
      
      if(parsa-- << equaler){
         
         parsa--;
         int64_t currentpoint = parsa.currentPoint();
         
         while(parsa.hasChar()){
         
            if(parsa-- == '"' || parsa == '\''){
               parsa.parseStringLiteralSmart();
            } else if(parsa == delimiter || parsa == bracketcloser){
               break;
            } else if(parsa == '('){
               parsa.parseCPlusUntilBracketEnd();
               parsa << ')';
            } else if(parsa == '['){
               parsa.parseCPlusUntilBracketEnd();
               parsa << ']';
            } else if(parsa == '{'){
               parsa.parseCPlusUntilBracketEnd();
               parsa << '}';
            } else {
               parsa.getChar();
            }
         }
         
         int64_t endpoint = parsa.currentPoint();
         func(attribname, parsa.getShringerFromTo(currentpoint, endpoint));
         
      } else {
         func(attribname, shringer()); // nothing on value
      }
      
      parsa-- << delimiter; // optional
      
   }
   
   return true;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseUntil(function<bool(int thechar)> lefunc, bool inclusive){

   string ret;
   
   while(true){
      int thechar = peekChar();
      
      if(thechar == EOF){
         break;
      } else if(!lefunc(thechar)){
         unicodeAppendStr(ret, thechar);
         accept();
      } else {
         // thechar == limiter
         if(inclusive){
            unicodeAppendStr(ret, thechar);
            accept();
         }
         break;
      }

   }
   
   return ret;

}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseUntilWhitespace(bool inclusive){
   string ret;
   
   while(true){
      int thechar = peekChar();
      
      if(thechar == EOF){
         break;
      } else if(!iswspace(thechar)){
         unicodeAppendStr(ret, thechar);
         accept();
      } else {
         // thechar == limiter
         if(inclusive){
            unicodeAppendStr(ret, thechar);
            accept();
         }
         break;
      }

   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseUntilChars(string limits, bool inclusive){

   string ret;
   
   while(true){
      int thechar = peekChar();
      
      if(thechar == EOF){
         break;
      } else {
      
         bool contained = false;
         int i;
         for(i = 0; i < limits.size(); i++){
            if(limits[i] == thechar){
               contained = true;
               break;
            }
         }
      
         if(!contained){
            unicodeAppendStr(ret, thechar);
            accept();
         } else {
            // thechar == limiter
            if(inclusive){
               unicodeAppendStr(ret, thechar);
               accept();
            }
         break;
         }
      }

   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseUntil(char limiter, bool inclusive){

   string ret;
   
   while(true){
      int thechar = peekChar();
      
      if(thechar == EOF){
         break;
      } else if(thechar != limiter){
         unicodeAppendStr(ret, thechar);
         accept();
      } else {
         // thechar == limiter
         if(inclusive){
            unicodeAppendStr(ret, thechar);
            accept();
         }
         break;
      }

   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
void EasyParser::expectNumbers(){
   if(!peekingAtNumbersMaybeNegative())
      error("expecting numbers here!");
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtNumbers(){
   return isdigit(peekChar());
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtNumbersMaybeNegative(){
   int thepeek = peekChar();
   return isdigit(thepeek) || (thepeek == '-' && isdigit(peekChar(1)));
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::peekingAtWhiteSpaces(){
   return iswspace(peekChar());
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser EasyParser::bs(){
   EasyParser ret = *this;
   ret.bunshin = true;
   ret.bunshinfail = false;
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::bsNum(int count, bool skipspaces){

   assert(bunshin);
   
   do_n_times(count,
      if(skipspaces)
         skipWhitespaces();
         
      if(!peekingAtNumbers() && peekChar() != '-'){
         bunshinfail = true;
         return *this;
      }
      parseFloat();
   );
   
   return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::bsLabel(int count, bool skipspaces){

   assert(bunshin);
   
   do_n_times(count,
      if(skipspaces)
         skipWhitespaces();
         
      string thelabel = parseLabel();
      if(thelabel == ""){
         bunshinfail = true;
         return *this;
      }
   );
   
   return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::bs(const shringer& sadasd){
   
   assert(bunshin);
   
   if(!acceptSeq(sadasd))
      bunshinfail = true;
   
   return *this;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::bs(char thechar){
   
   assert(bunshin);
   
   if(!accept(thechar))
      bunshinfail = true;
   
   return *this;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::sw(){
   skipWhitespaces();
   return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::swen(){
   skipWhitespacesExceptNewline();
   return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::operator--(int){
   skipWhitespaces();
   return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::operator==(int thechar){

   if(bunshin && bunshinfail)
      return false;

   if(_caseless){
      return towlower(peekChar()) == towlower(thechar);
   } else {
      return peekChar() == thechar;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::operator==(const shringer& theword){

   if(bunshin && bunshinfail)
      return false;

   return isPeekingAt(theword);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::operator!=(int thechar){

   if(bunshin && bunshinfail)
      return false;

   if(_caseless){
      return towlower(peekChar()) != towlower(thechar);
   } else {
      return peekChar() != thechar;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::operator!=(const shringer& theword){

   if(bunshin && bunshinfail)
      return false;

   return !isPeekingAt(theword);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::operator<<(int thechar){

   if(bunshin && bunshinfail)
      return false;

   return accept(thechar);
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::operator<<(const shringer& theword){

   if(bunshin && bunshinfail)
      return false;

   return acceptSeq(theword);
}
//////////////////////////////////////////////////////////////////////////////////////////
string& EasyParser::smartMatchResultVecElt(int thenum){
   while(thenum >= matchresultvec.size())
      matchresultvec.push_back(string());
   
   return matchresultvec[thenum];
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::operator()(const shringer& thevarname){
   auto it = matchresultmap.find(thevarname.toString());
   if(it == matchresultmap.end())
      return "";
   
   return it->second;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::operator()(int thevarindex){
   if(thevarindex >= matchresultvec.size())
      return "";
   
   return matchresultvec[thevarindex];
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::trySmartMatch(const shringer& thecommand){
   
   matchresultvec.clear();
   matchresultmap.clear();
   
   EasyParser parsa(thecommand);
   
   while(parsa.hasChar()){
      
      int curchar = parsa.peekChar();
      
      if(iswspace(curchar)){
         parsa.accept();
         continue; // yeah, back to above
      }
      
      if(curchar == '\"'){
         string literal = parsa.parseStringLiteral();
         // cool
         if(!acceptSeq(literal))
            return false;
        
      } else if (isupper(curchar)){
                  
         string thelabel = parsa.parseLabel();
         
         if(!acceptSeqCased(thelabel, false))
            return false;
         
         // otherwise all good, no?
         
      } else if (islower(curchar)){
      
         string thelabel = parsa.parseLabel();
         
         if(!acceptSeqCased(thelabel, true)) // caseless!
            return false;
      
      } else if (parsa.peekingAtNumbersMaybeNegative()){
      
         string numstring = parsa.parseNumberString(true);
         
         string curnumstring = parseNumberString(true);
         
         if(numstring != curnumstring)
            return false;
         
         // just try to match it!
         
      } else if (curchar == '('){
         
         string thevar;
      
         // parse the actual element
         if(peekingAtLabelStart()){
            thevar = parseLabel();
         } else if (peekingAtNumbersMaybeNegative()){
            thevar = parseNumberString(true);
         } else {
            thevar = string(1, curchar);
            accept(); // accept a cahr
            // accept anything, really
         }
         
         // parse the variable name etc, and assign it
         parsa.accept();
         
         if(parsa.peekingAtLabelStart()){
            string varname = parsa.parseLabel();
            matchresultmap[varname] = thevar;
         } else if (parsa.peekingAtNumbers()){
            int thenum = parsa.parseNumber();
            smartMatchResultVecElt(thenum) = thevar;
         } else {
            // what, expecting stuff!
            serror = Serror("Expecting a proper variable name or index!");
            serror.add("@ [" + __toString(linenum) + ":" + __toString(colnum) + "]");
            return false;
         }
         
         if(!(parsa << ')')){
            serror = Serror("Hey, expecting a closing bracket!");
            serror.add("@ [" + __toString(linenum) + ":" + __toString(colnum) + "]");
            return false;
         }
         
      } else {
      
         if(parsa == '\\'){
            parsa.accept(); // yep, escape it
            // some sort of punctuation, yeah!
            
            if(!parsa.hasChar()){
               serror = Serror("a slash at the end of matchin!");
               serror.add("@ [" + __toString(linenum) + ":" + __toString(colnum) + "]");
               return false;
            }
            
            curchar = parsa.getChar();
         }
         
         parsa.accept();
         
         if(!accept(curchar)){
            return false;
         }
         
      }
      
      // excellent
      
   }
   
   return true; // looks like all ist gut

}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::operator|(const shringer& thecommand){
   
   EasyParser dupo = *this;
   bool succ = dupo.trySmartMatch(thecommand);
   
   if(succ){
      swap(*this, dupo); // yep
      return true;
   } else {
      // beh
      return false;
   }
   
}
//////////////////////////////////////////////////////////////////////////////////////////
bool EasyParser::operator|=(const shringer& thecommand){
   EasyParser dupo = *this;
   bool succ = dupo.trySmartMatch(thecommand);
   
   if(succ){
      swap(matchresultvec, dupo.matchresultvec);
      swap(matchresultmap, dupo.matchresultmap);
      swap(serror, dupo.serror);
      return true;
   } else {
      // beh
      return false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseAnsiCComment(){
   
   if(!acceptSeq("/*"))
      return "";
   
   // well, got the /* bit
   
   string ret;
   
   while(hasChar()){
      
      int curchar = peekChar();
      
      if(curchar == '*' && acceptSeq("*/")){
         return ret;
      } else {
         accept();
         unicodeAppendStr(ret, curchar);
      }
      
   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
namespace Shing{

void CPlusArglist::addChar(int lechar){
   rawstring.append(1, lechar);
   
   if(args.size() == 0)
      args.push_back(string());
   args.back().append(1, lechar);
}

void CPlusArglist::addComma(){
   rawstring.append(1, ',');
   if(args.size() == 0)
      args.push_back(string());
   args.push_back(string()); // new arg
}

}; // end namespace Shing
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCPlusOperandReverse(){
   splaicht(cplus_reverse_mode);
   return parseCPlusOperand();
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCPlusOperand(){
   
   string ret;
   EasyParser& parsa = *this;
   
   int64_t startpoint = parsa.currentPoint();
   gplaicht(cplus_operand_noprefix);
   gplaicht(cplus_parser_nosquare_brackets);
   
   gplaicht(cplus_reverse_mode);
   
   
   if(cplus_reverse_mode){
   

      while(parsa.hasChar()){
         int thechar = parsa.peekChar();
         int secondchar = parsa.peekChar(1);
         
         if(thechar == '-' && secondchar == '-'){
            parsa.getChar();
            parsa.getChar();
         } else if (thechar == '+' && secondchar == '+'){
            parsa.getChar();
            parsa.getChar();
         } else {
            break; // finished prefixes
         }
      }
      
      
      // well
      bool was_on_label = false;
      bool got_label = false;
      while(parsa.hasChar()){
         int thechar = parsa.peekChar();
         
         if(!was_on_label && parsa << ')'){
            parsa.parseCPlusUntilBracketEnd();
            parsa << '(';
            was_on_label = false;
         } else if (!was_on_label && !cplus_parser_nosquare_brackets && parsa << ']'){
            parsa.parseCPlusUntilBracketEnd();
            parsa << '[';
            was_on_label = false;
         } else if (parsa << '.'){
            parsa.parseCPlusOperand();
            was_on_label = false;
         } else if (parsa << "\\\\"){
            parsa.parseCPlusOperand();
            was_on_label = false;
         } else if (parsa << ">-"){
            parsa.parseCPlusOperand();
            was_on_label = false;
         } else if (parsa << "::"){
            parsa.parseCPlusOperand();
            was_on_label = false;
            // a sub expression, you might say
         } else if (!was_on_label && isalpha(thechar)){
            parsa.parseLabel();
            was_on_label = true;
            got_label = true;
         } else {
            break; // nothing more to do here
         }
         
      }
      
      while(parsa.hasChar()){
         int thechar = parsa.peekChar();
         
         if(thechar == '-'){
            parsa.getChar();
         } else if (thechar == '!'){
            parsa.getChar();
         } else if (thechar == '~'){
            parsa.getChar();
         } else if (thechar == '+'){
            parsa.getChar();
         } else if (thechar == '*'){
            parsa.getChar();
         } else {
            break; // finished prefixes
         }
      }
      
   
   } else {
   
      // parse prefixes
      // however many prefixing operators
      if(!cplus_operand_noprefix){
         while(parsa.hasChar()){
            int thechar = parsa.peekChar();
            
            if(thechar == '-'){
               parsa.getChar();
            } else if (thechar == '!'){
               parsa.getChar();
            } else if (thechar == '~'){
               parsa.getChar();
            } else if (thechar == '+'){
               parsa.getChar();
            } else if (thechar == '*'){
               parsa.getChar();
            } else {
               break; // finished prefixes
            }
         }
      }
      
      
      int nextchar = parsa.peekChar();
      if(parsa--.peekingAtLabelStart()){
         parsa.parseLabel();
         
         while(parsa--.hasChar()){
            if(parsa << '('){
               parsa.parseCPlusUntilBracketEnd();
               parsa << ')';
            } else if (!cplus_parser_nosquare_brackets && parsa << '['){
               parsa.parseCPlusUntilBracketEnd();
               parsa << ']';
            } else if (parsa << '.'){
               splaicht(cplus_operand_noprefix);
               parsa.parseCPlusOperand();
            } else if (parsa << "\\\\"){
               splaicht(cplus_operand_noprefix);
               parsa.parseCPlusOperand();
            } else if (parsa << "->"){
               splaicht(cplus_operand_noprefix);
               parsa.parseCPlusOperand();
            } else if (parsa << "::"){
               splaicht(cplus_operand_noprefix);
               parsa.parseCPlusOperand();
               // a sub expression, you might say
            } else {
               break; // nothing more to do here
            }
         }
         
      } else if(parsa.peekingAtNumbers()){
         parsa.parseFloat(); // just parse any and all possible floats
         if(parsa << 'f'){
            // maybe a float
         } else if (parsa << 'l'){
            // maybe a long
         } else if (parsa << '_'){
            parsa.parseLabel();
         }
      } else if (nextchar == '\''){
         // it's a char
         parsa.parseStringLiteralSmart();
      } else if (nextchar == '"'){
         // it's a label, lel
         parsa.parseStringLiteralSmart();
      }
      
      while(parsa--.hasChar()){
         int thechar = parsa.peekChar();
         int secondchar = parsa.peekChar(1);
         if(thechar == secondchar && secondchar == '+'){
            parsa.accept();
            parsa.accept();         
         } else if(thechar == secondchar && secondchar == '+'){
            parsa.accept();
            parsa.accept();
         } else {
            break;
         }
      }
      
   
   }

   
   

   
   // parse postfix if any
   if(!cplus_reverse_mode){

   } else {
      // on reverse, so..

   }
   
   int64_t newcurpoint = parsa.currentPoint();
   return parsa.getShringerFromTo(startpoint, newcurpoint).toString();
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCPlusUntilBracketEnd(function<bool(int, EasyParser&)> enderfunc){
   EasyParser& parsa = *this;
   
   int bracketstack = 0;
   
   string ret;
   
   while(parsa.hasChar()){
      int thechar = parsa.peekChar();
      
      bool pregot = false;
      
      if(enderfunc && bracketstack == 0 && enderfunc(thechar, parsa)){
         break; // don't accept, yeah
      } else if(thechar == '"' || thechar == '\''){
         // fudge, gotta think about this eh>?
         ret.append(parsa.parseStringLiteralSmartReturnRaw());
         pregot = true;
      } else if(thechar == '('){
         
         bracketstack++;
         
      } else if (thechar == ')'){
         
         if(bracketstack == 0)
            break;
            
         bracketstack--;
         
      } else if (thechar == '['){
         bracketstack++;
      } else if (thechar == ']'){
         if(bracketstack == 0)
            break;
         bracketstack--;
      } else if(thechar == '/'){
         if(parsa.peekChar(1) == '/'){
            break; // comment time
         }
         
      } else {
      }
      
      // assuming accepting it
      if(!pregot){
         parsa.getChar();
         ret.append(1, thechar);
      }
      
   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
shringer EasyParser::parseUntilLineComments(const string& starterpattern){
   
   EasyParser& parsa = *this;
   if(starterpattern.size() == 0){
      return parsa--.parseUntilEnd();
   }
   
   int commentfirstchar = starterpattern[0];
   
   int64_t startpoint = parsa--.currentPoint();
   
   while(parsa--.hasChar()){
      int nextchar = parsa.peekChar();
      if(nextchar == commentfirstchar && parsa == starterpattern){
         break;
      } else if(nextchar == '\'' || nextchar == '"'){
         parsa.parseStringLiteralSmart();
      } else {
         parsa.getChar();
      }
   }
   
   return parsa.getShringerFromTo(startpoint, parsa.currentPoint());
   
   
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCodeSmart(char ender1, char ender2, char ender3, char ender4){
   return parseCPlusUntilEnderOrCommentOrBracketEnd(ender1, ender2, ender3, ender4);
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCPlusUntilEnderOrCommentOrBracketEnd(char theender, char secondender, char thirdender, char fourthender){
   EasyParser& parsa = *this;
   
   int bracketstack = 0;
   
   string ret;
   
   while(parsa.hasChar()){
      int thechar = parsa.peekChar();
      
      bool pregot = false;
      
      if(thechar == '"' || thechar == '\''){
         // fudge, gotta think about this eh>?
         ret.append(parsa.parseStringLiteralSmartReturnRaw());
         pregot = true;
         
      } else if (bracketstack == 0 && (thechar == theender || thechar == secondender || thechar == thirdender || thechar == fourthender)){
         break;
         
      } else if(thechar == '(' || thechar == '{' || thechar == '['){
         bracketstack++;
         
      } else if (thechar == ')' || thechar == ']' || thechar == '}'){
         
         if(bracketstack == 0)
            break;
            
         bracketstack--;


         
      } else if(thechar == '/'){
         if(parsa.peekChar(1) == '/'){
            break; // comment time
         }

         
      } else {
      }
      
      // assuming accepting it
      if(!pregot){
         parsa.getChar();
         ret.append(1, thechar);
      }
      
   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCPlusUntilSemicolonOrCommentOrBracketEnd(){
   return parseCPlusUntilEnderOrCommentOrBracketEnd(';');
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCPlusUntilCommaOrCommentOrBracketEnd(){
   return parseCPlusUntilEnderOrCommentOrBracketEnd(',');
   
}
//////////////////////////////////////////////////////////////////////////////////////////
Shing::CPlusArglist EasyParser::parseCPlusArglistContents(){
   EasyParser& parsa = *this;
   
   Shing::CPlusArglist ret;
   
   int bracketstack = 0;
   
   while(parsa.hasChar()){
      int thechar = parsa.peekChar();
      if(thechar == ')'){
         
         if(bracketstack == 0){
            return ret;
         } else {
            parsa << ')';
            
            bracketstack--;
            if(bracketstack == 0){
               ret.rawstring.append(1, ')');
               return ret;
            } else {
               ret.addChar(')');
            }
            
         }
      } else if (thechar == '('){
         parsa << '(';
         if(bracketstack == 0){
            ret.rawstring.append(1, '(');
         } else {
            ret.addChar('(');
         }
         bracketstack++;
      } else if (thechar == ','){
         parsa.getChar();
         if(bracketstack == 1){
            ret.addComma();
         } else {
            ret.addChar(',');
         }
      } else {
         int thechar = parsa.getChar();
         ret.addChar(thechar);
      }
   }
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
Shing::CPlusDeclSig EasyParser::parseCPlusDeclSig(){
   EasyParser parsa = *this; // temporary parsa, only accepts if true
   
   Shing::CPlusDeclSig ret;
   ret.is_valid = true;
   
   if(parsa-- << "static"){
      ret.is_static = true;
   }
   
   ret.rettype = parsa--.parseCPlusTypeString();
   if(parsa-- << "::"){
      EasyParser subparsa(ret.rettype);
      ret.containingclass = subparsa.parseLabel(); // ignoring template arguments for now, eh?
      
      ret.rettype = "";
      
      ret.name = parsa--.parseCPlusMethodName();
      
   } else if(parsa--.peekingAtLabelStart()){
      
      ret.name = parsa--.parseCPlusMethodName();
      
      if(parsa-- << "::"){
         ret.containingclass = ret.name;
         ret.name = parsa--.parseCPlusMethodName(); // sounds good, yeah
         
      }
      
   } else {
      // no way, this won't work
      ret.is_valid = false;
      return ret;
   }
   
   if(ret.name.size() == 0){
      ret.is_valid = false;
      return ret;
   }
   
   if(parsa-- == '('){
      ret.arglist = parsa.parseCPlusArglistContents();
      ret.is_method = true;
      
      if(parsa-- << "const"){
         ret.is_const = true;
      }
   }
   
   *this = parsa;
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCPlusMethodName(){
   auto& parsa = *this;
   
   if(parsa == '~'){
      EasyParser tempa = parsa;
      tempa << '~';
      string deconst = tempa--.parseLabel();
      if(deconst != ""){
         parsa = tempa;
         return "~" + deconst;
      }
   }
   
   string ret = parsa--.parseLabel();
   if(ret == "operator"){
      
      int nextchar = parsa--.peekChar();
      switch(nextchar){
         case '+':
         case '-':
            parsa.accept(); ret.append(1, nextchar);
            if(parsa << nextchar){ ret.append(1, nextchar); }
            else if(parsa << '='){  ret.append(1, '='); }
            else if(parsa << '>'){  ret.append(1, '>'); }
            break;
         case '!':
         case '*':
         case ',':
         case '/':
         case '%':
         case '=':
         case '^':
            parsa.accept(); ret.append(1, nextchar);
            if(parsa << '='){  ret.append(1, '='); }
         
            break;
            break;
         case '|':
         case '&':
         case '<':
         case '>':
            parsa.accept(); ret.append(1, nextchar);
            if(parsa << nextchar){ ret.append(1, nextchar); }
            if(parsa << '='){  ret.append(1, '='); }
         
            break;
         
         case '(':
            if(parsa << "()"){
               ret.append("()");
               // cool
            }
            
            break;
         case '[':
            if(parsa << "[]"){
               ret.append("[]");
            }
            break;
         case ' ':
            if(parsa-- << "bool"){
               ret.append(" bool");
            }
      }
      
      
   }
   
   return ret;
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCPlusTypeString(){
   
   if(!peekingAtLabelStart()){
      return "";
   }
   
   // maybe get qualifiers too
   
   auto parsa = *this; // parsa is a temp
   
   string toprepend;
   while(true){
      // these are qualifiers eh
      if(parsa << "const"){
         toprepend.append("const ");
         parsa--;
      } else if (parsa << "typename"){
         toprepend.append("typename ");
         parsa--;
      } else {
         break;
      }
   }
   
   // always starts with a label
   string ret = toprepend + parsa.parseLabel();
   
   if(ret == "")
      return ""; // can't parse a name
      
   *this = parsa; // not empty, so we accept it
   
   while(true){
      if(parsa-- << "::"){
      
         string actualtype = parsa--.parseLabel();
         if(actualtype == ""){
            return ret; // ignore this then
         }
         
         ret.append("::");
         ret.append(actualtype);
         
         *this = parsa; // take it, I guess
         parsa = *this;
         
      } else if (parsa-- << '<'){
      
         vector<char> braces; // either < or (
         
         // assume it is part of the type
         braces.push_back('<');
         ret.append(1, '<');
         
         while(parsa--.hasChar()){
            
            int thechar = parsa.peekChar();
            
            if(parsa == '<'){
               braces.push_back('<');
               parsa.accept();
               ret.append(1, '<');
            } else if (parsa == '('){
               braces.push_back('(');
               parsa.accept();
               ret.append(1, '(');
            } else if (parsa == '>'){
               if(braces.size() == 0 || braces.back() != '<'){
                  return ret; // fail with this one
               }
               
               braces.pop_back();
               parsa.accept();
               ret.append(1, '>');
               
            } else if (parsa == ')'){
               if(braces.size() == 0 || braces.back() != '('){
                  return ret; // fail with this one
               }
               
               braces.pop_back();
               parsa.accept();
               ret.append(1, ')');
               
               
            } else if (isalpha(thechar) || thechar == '_'){
               
               // parse label
               string label = parsa.parseLabel();
               ret.append(label); // yeah
               
            } else {
               
               int thechar = parsa.getChar();
               ret.append(Morestring::unicodify(thechar));
               
            }
            
            if(braces.size() == 0){
               break;
            }
            
         }
         
      } else {
         break;
      }
   
   }
   
   *this = parsa; // yeah, officialize and accept
   
   while(true){
      
      if(parsa-- << '*'){
         ret.append(1, '*');
         *this = parsa;
      } else if (parsa << '&'){
         ret.append(1, '&');
         *this = parsa;
      } else {
         break;
      }
      
   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCPlusComment(){

   if(!acceptSeq("//")){
      return "";
   }
   
   // well, got the slash slash
   
   string ret;
   
   bool newlineescaped = false;
   
   while(hasChar()){
      int curchar = peekChar();
      
      if(curchar == '\n'){
         accept();
         if(newlineescaped){
            unicodeAppendStr(ret, curchar);
         } else {
            break; // done
         }
      } else if (curchar == '\\'){
         newlineescaped = true;
         accept();
         unicodeAppendStr(ret, curchar);
      } else {
         newlineescaped = false;
         accept();
         unicodeAppendStr(ret, curchar);
      }
      
   }
   
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
string EasyParser::parseCComment(){
   
   skipWhitespaces();
      
   string ret;
   ret = parseCPlusComment();
   if(ret != "")
      return ret;
   
   ret = parseAnsiCComment();
   return ret;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::skipCComment(){

   int acceptcount = skipWhitespaces();
   
   if(!acceptSeq("//")){
      return acceptcount;
   }
   
   // well, got the slash slash
   
   bool newlineescaped = false;
   
   while(hasChar()){
      int curchar = peekChar();
      
      if(curchar == '\n'){
         accept();
         acceptcount++;
         if(newlineescaped){
         } else {
            break; // done
         }
      } else if (curchar == '\\'){
         newlineescaped = true;
         accept();
         acceptcount++;
      } else {
         newlineescaped = false;
         accept();
         acceptcount++;
      }
      
   }
   
   return acceptcount;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
EasyParser& EasyParser::sc(){
   skipCComment();
   return *this;
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::skipWhitespacesExceptNewline(){
   
   int64_t acceptcount = 0;
   
   while(true){
      int curchar = peekChar();
      
      if(iswspace(curchar) && curchar != '\n'){
         acceptcount++;
         accept();
      } else {
         break;
      }

   }
   
   return acceptcount;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
int EasyParser::skipWhitespaces(){
   
   int64_t acceptcount = 0;
   
   while(true){
      int curchar = peekChar();
      
      if(iswspace(curchar)){
         acceptcount++;
         accept();
      } else {
         break;
      }

   }
   
   return acceptcount;
   
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
BigEndianEasyParser::BigEndianEasyParser(const shringer& themoo):
   EasyParser(themoo){
   bigEndian();
}
//////////////////////////////////////////////////////////////////////////////////////////
BigEndianEasyParser::BigEndianEasyParser(const string& themoo, int64_t offset):
   EasyParser(themoo, offset){
   bigEndian();
}
//////////////////////////////////////////////////////////////////////////////////////////
BigEndianEasyParser::BigEndianEasyParser(const char * thestr, int64_t thelength):
   EasyParser(thestr, thelength){
   bigEndian();
}
//////////////////////////////////////////////////////////////////////////////////////////
//BigEndianEasyParser::BigEndianEasyParser(istream& istr):
//   EasyParser(istr){
//   bigEndian();
//}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
UTF8EasyParser::UTF8EasyParser(const shringer& themoo):
   EasyParser(themoo){
   disableUnicode(false);
}
//////////////////////////////////////////////////////////////////////////////////////////
UTF8EasyParser::UTF8EasyParser(const string& themoo, int64_t offset):
   EasyParser(themoo, offset){
   disableUnicode(false);
}
//////////////////////////////////////////////////////////////////////////////////////////
UTF8EasyParser::UTF8EasyParser(const char * thestr, int64_t thelength):
   EasyParser(thestr, thelength){
   disableUnicode(false);
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

