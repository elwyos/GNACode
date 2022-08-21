
#ifndef __EasyParser__definizione__
#define __EasyParser__definizione__
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "membuf.hpp"
#include <stdint.h>
#include <map>
#include "serror.hpp"
#include "ptrie.hpp"
#include "classdef.hpp"
//////////////////////////////////////////////////////////////////////////////////////////
namespace std{}; using namespace std;
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
/*
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Easy Parser

Parses everything : ascii strings into ints, tokens, etc
Also parses binary! ints, floats, etc

(CyanoTransAbsorber has been infused into it).


Same with EasyWriter, you can do 
   EasyParser::bige parsa(thestr); // bige means bigEndian!

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#include "easyexpression.hpp"
#include "phonenumber.hpp"
#include "animrange.hpp"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#include "easy_cplusparse.hpp"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
class IntOrFloat{
   public:
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool isint;
      int intret;
      float floatret;
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      IntOrFloat();
      IntOrFloat(int theint);
      IntOrFloat(float thefloat);
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      float asfloat();
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
};
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
class BigEndianEasyParser;
class UTF8EasyParser;
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
class EasyParser{
   public:
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool bigendian; // is it? is it?
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      const char * input;
      int64_t curpoint; // also used by istre, to indicate how many characters read
      int64_t inputlength;
      int64_t acceptcount_unicode; // how many characters accepted, unicode-wise? only valid on unicode
      Shing::Attmeta * attmeta; // optional, if input is not NULL
      deque<int> unicode_lookbuffer;
      int unicode_bufferendpoint; // in relation to input. input[bufferendpoint] is where peeking starts next
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      istream * istre; // if there's an istream
      string lookbuffer;
      int64_t lookpoint;
      bool ended;
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // parsing data
      int64_t linenum;
      int64_t colnum;
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool _unicode; // AKA UTF8 Mode
      bool _unicode_disabled; // mostly for sanity checking so no mode mixing
      bool _caseless;
      int unicode_lastaccepted; // last accepted char
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool bunshin; // is it bunshin?
      bool bunshinfail; // did the bunshin fail when trying to accept stuff?
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      typedef BigEndianEasyParser bige;
      typedef UTF8EasyParser utf8mode;
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      vector<string> matchresultvec;
      map<string, string> matchresultmap;
      Serror serror; // matching errors
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      //EasyParser(Shing::Membuf& membuf);
      //EasyParser(Shing::Membuf* membuf);
      //EasyParser(const string& themoo);
      EasyParser(Shing::Attstring& thestring);
      EasyParser(const shringer& shring, Shing::Attmeta * themeta = NULL);
      EasyParser(const string& themoo, int64_t offset, Shing::Attmeta * themeta = NULL);
      EasyParser(const char * thestr, int64_t thelength, Shing::Attmeta * themeta = NULL);
      // EasyParser(istream& istr);
      EasyParser(); // an empty easyparser!
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool isNull(); // easyparser can actually be 
      bool exists();
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // Reminder : Be careful with copy constructors, destructors etc.
      // Because we like to do Parsa b = x; x.doSomething(); if(heavensalign) b = x;
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      void initCharStar(const char* theinput, int64_t thelen, Shing::Attmeta * theattmeta);
      void initIStream(istream& istr);
      void initCommon();
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // stream specific functions, (private) goes without saying
      bool preloadChars(int numtopreload);
      void maybeFlushLookpoint(bool force = false); // don't force flush by default
      void emptyLookBuffer();
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // cheat, read and potentially modify the rest of the stream. only guaranteed to
      // work with membuf/secubuf or char* stuff.
      string getLeftovers(); // faster, dyrect string creation
      shringer getLeftoverBuffer();
      shringer getLeftoverBufferUntil(int64_t untilpos); // untilpos corresponds to input space pos
      shringer getShringerFromTo(int64_t startpos, int64_t endpos);
      shringer getShringerFrom(int64_t startpos);
         // these are non-unicode buffers
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // checking attribute meta, yep
      bool checkMetaSince(int theflag, int prevpos);
      bool checkMetaJustBecame(int theflag, int relindex = 0);
      bool checkMetaJustBecameNot(int theflag, int relindex = 0);
      bool checkMeta(int theflag, int relindex = 0);
      bool checkMetaAbsolute(int theflag, int theindex);
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // easy shortcuts for checking meta's
      bool justGotBold();
      bool justGotItalic();
      bool justGotNotBold();
      bool justGotNotItalic();
      bool boldSince(int prevpos);
      bool italicSince(int prevpos);
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // makes acceptSeq, and acceptChar case insensitive
      EasyParser& caseless(bool truth = true);
      EasyParser& disableUnicode(bool truth = true);
      bool unicodeIsDisabled();
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool hasCharNoUnicode(int64_t numofchars = 1);
      bool hasChar(int64_t numofchars = 1);
      operator bool() { return hasChar(); }
      int getCharNoUnicode();
      int getChar();
      int peekCharNoUnicode(int64_t peekindex = 0);
      int peekChar(int64_t peekindex = 0, bool nounicode = false);
      int lastCharNoUnicode(int64_t theindex = 0);
      int lastChar(int64_t shiftindex = 0); 
         // 0 is last character, -1 is the one before that etc. for unicode, only 0 is acceptable.
      string takeHexes(int64_t maxsize); // takes a certain amount of hex chars (0-9a-fA-F)
      string takeStringNoUnicode(int64_t maxsize);
      string peekString(int64_t maxsize);
      string peekStringNoUnicode(int64_t maxsize);
      bool isPeekingAt(const shringer& theseq, bool nounicode = false);
      bool isPeekingAtCased(const shringer& theseq, bool caselessness, bool nounicode = false);
      bool isPeekingAtNoUnicodeCased(const shringer& theseq, bool caselessness);
      bool accept();
      bool accept(int thechar);
      bool acceptCharNoUnicode(int expected);
      bool acceptChar(int expected, bool nounicode = false);
      bool acceptNChars(int then, bool nounicode = false);
      void mustAcceptChar(char thechar, bool nounicode = false);
      bool acceptWholeLabel(const shringer& thestring);
      bool accept(const shringer& theseq, bool nounicode = false);
      bool acceptSeq(const shringer& theseq, bool nounicode = false);
      bool acceptSeqCased(const shringer& theseq, bool caselessness, bool nounicode = false);
      bool acceptSeqCasedNoUnicode(const shringer& theseq, bool caselessness);
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      vector<int> parseEmojiCharacter();
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      int64_t segmentSize();
      int64_t currentPoint(); // non unicode character count. aka 8bit distance from start
      bool finished(); // nothing more to parse
      bool allWhitesSinceBeginningOfLine(); // useful for cehcking C macros
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      void throwError(const string& errmsg); // throws a serror for whatever
      bool error(const string& errmsg, bool assertquit = true, bool exitquit = false);
      void erroust(const string& errmsg, bool assertquit = false, bool exitquit = false);
      string getErrorLineMarkup(); // get the marked up version of the error.
      string getErrorLineMarkup(int64_t& errorpoint); // returns the index position of the error
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` -
      // binary data absorbption / osmosis ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , `
      // , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` -
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      EasyParser& bigEndian();
         // sets binary data absorption functions to be big endian, 
         // when dealing with numbers/floats
         // bigendian is false by default
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // if it gets the char, it'll absorb value. otherwise it'll set it to defaultvalue
      /*
      bool qmcrd(char code, int32_t& value, int32_t defaultvalue = 0);
      bool qmcrd(char code, uint32_t& value, uint32_t defaultvalue = 0);
      bool qmcrd(char code, int16_t& value, int16_t defaultvalue = 0);
      bool qmcrd(char code, float& value, float defaultvalue = 0);
      bool qmcrd(char code, uint8_t& value, uint8_t defaultvalue = 0);
      bool qmcrd(char code, int8_t& value, int8_t defaultvalue = 0);
      bool qmcrd(char code, TileIndex& value, const TileIndex& defaultvalue = TileIndex());
      bool qmcrd(char code, Point2D& value, const Point2D& defaultvalue = Point2D());
      bool qmcrd(char code, Coord3D& value, const Coord3D& defaultvalue = Coord3D());
      bool qmcrd(char code, ColorInfo& value, const ColorInfo& defaultvalue = ColorInfo());
      */

      bool qmcr(char code, int32_t& value);
      bool qmcr(char code, uint32_t& value);
      bool qmcr(char code, int16_t& value);
      bool qmcr(char code, float& value);
      bool qmcr(char code, uint8_t& value);
      bool qmcr(char code, int8_t& value);
      bool qmcr(char code, TileIndex& value);
      bool qmcr(char code, Point2D& value);
      bool qmcr(char code, Coord3D& value);
      bool qmcr(char code, ColorInfo& value);
      
      // returns true if the code was found. the qmcr version doesn't set default value
      // probably is the most preferable mdoe but yeah...
      // make sure you end the qmcb's with a char to mark the end of the codes...

      
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // auto absorb - calls absorbInt, absorbFloat, absorbChar etc depending on type
      bool ausorb(string& ret);
      bool ausorb(bool& thebool);
      bool ausorb(float& theint);
      bool ausorb(double& theint);
      bool ausorb(uint64_t& theint);
      bool ausorb(int64_t& theint);
      bool ausorb(int32_t& theint);
      bool ausorb(uint32_t& theint);
      bool ausorb(uint16_t& theint);
      bool ausorb(int16_t& theint);
      bool ausorb(uint8_t& theint);
      bool ausorb(int8_t& theint);
      bool ausorb(char& theint);
      
      bool ausorb(TileIndex& thetile);
      bool ausorb(Point2D& thepoint);
      bool ausorb(Coord3D& thepoint);
      bool ausorb(ColorInfo& thecolor);

      
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // no unicode mode is imperative
      bool absorbBool(bool& boolret);
      bool absorbChar(char& charret);
      bool absorbByte(char& charret);
      bool absorbByte(int8_t& charret); 
      bool absorbUByte(uint8_t& charret); // unsigned mode
         // aka absorbChar, but its confusing with acceptChar
         // absorbs 1 byte (8 bits). basically calls getChar.
      bool absorbShort(int16_t& theshort);
      bool absorbShort(uint16_t& theshort);
         // absorbs a 2 byte short (16 bit) into the short. questionable byte ordering.
      bool absorbInt(int32_t& theint);
      bool absorbInt(uint32_t& theint);
         // absorbs a 32 bit integer (4 bytes) into theint.
         // uses morestring's byte ordering
      bool absorbLong(int64_t& thelong);
      bool absorbUnsignedLong(uint64_t& thelong);
         // absorbs 64 bit
      void absorbIntAndExpect(int32_t theexected);
         // absorbs int like above, and asserts that the value is as expected
      bool absorbFloat(float& thefloat);
         // same like above, 32 bit float (4 bytes) into the float.
         // byte ordering still questionable
      bool absorbDouble(double& thedouble);
         // same like absorbFloat, but 64 bit double precision
      bool absorbMCVarInt(int32_t& theint);
         // variable-length integer (minecraft inspired)
         // tested on shingmisc april 20th 2019
      bool absorbMCVarLong(int64_t& theint);
         // variable-length long (minecraft inspired)
         // tested on shingmisc april 20th 2019
      bool absorbString(int length, string& ret);
         // absorbs ascii string. must specify length, though
      bool absorbNumberStringPair(string& ret);
         // absorbs an integer (absorbInt) and uses that length to absorb ascii string
         // and returns it to ret
      shringer absorbNumberStringPairShringer();
         // returns a shringer reference instead
      shringer absorbBuffer(int segmentsize);
         // returns a shringer reference o memory segment. advances the parser too
         // will return NULL
      bool absorbBSONString(string& ret);
         // like absorbNumberStringPair, but BSON style. the encoded string has a \0 in the end.
         // so it gets a string of length - 1 into ret
      bool absorbBSONCstring(string& ret);
         // absorbs chars until 0x00 is found
      bool absorbIntoMembuf(int length, Shing::Membuf& buf);
         // copies contents to memory buffer
      bool absorbIntoMemory(char * ptr, int length);
         // absorbs into a location in memory, with length given
      EasyParser absorbSubparser(int64_t segmentsize);
         // absorbs the next segment as an easyparser. saves effort with no new string creation
      EasyParser absorbNumberStringPairSubparser();
         // this can be used instead of absorbUmberStringPair for something more efficient
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // and the coolest function of them all. not quite endian safe though.
      template<class X>
      bool absorbInto(X& xoo){
         return absorbIntoMemory((char*)&xoo, sizeof(X));
      }
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` -
      // parsers and testers, works mainly in an ascii way ` , ` , ` , ` , ` , ` , ` , `
      // , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` , ` -
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool peekingAtPhoneNumber();
      Shing::PhoneNumber parsePhoneNumber();
      int64_t parseNumber();
      int64_t parseCommaedNumber();
      string parseIPAddress(); // ipv4, and no ports
      int parseNumberMaybeNegative(); // handles the -ve sign
      float parseFloat(); // maybe negative
      IntOrFloat parseIntOrFloat();
      IntOrFloat parseIntOrFloatMaybeNegative();
      string parseIntOrFloatString(bool& isfloat);
      string parseNumberString(bool maybeneg = false);
      string parseDottedNumberString();
      Shing::flxoat parseFlxoat(); // cross-platform, no floating point math
      void expectNumbers(); // errors otherwise
      bool peekingAtNumbers(); bool paNUM() { return peekingAtNumbers(); }
      bool peekingAtNumbersMaybeNegative();
      Shing::AnimRange parseAnimRange(); // for mars animations. name (start-end) combo, where start/end is in percentages
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      void parseRomanEx(string& ret, int& thenumber, bool allownocaps);
      string parseRomanNumeralString(bool allownocaps);
      int parseRomanNumeral(bool allownocaps);
      bool peekingAtRoman(bool allowlowcaps);
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool peekingAtOctal();
      int parseOctal(int maxlen = 0);
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool peekingAtHex();
      int parseHex(int maxlen = 0);
      string parseHexString(int maxlen = 0);
      shingcolor parseColor();
         // parses a hex string, of 8 max length
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      SDate parseDateSmartish(char day_or_month_first = 'm');
         // either 'm' or 'd'. 'm'onth first is american sillyness, but anglosphere loves it so..
         // but if year is first, then it's yyyy mm dd for everyone in the world
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // color code: 196%0.6 or #fff (196 is 196_xcoel, with % 0.3 as alpha value)
      // 196_xcoel%0.3 also works
      bool peekingAtColorCode();
      shingcolor parseColorCode();
      ColorInfo parseColorInfo(); // this one lets you have alpha over 1
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool peekingAtAnyNumber();
      int64_t parseAnyNumber();
      string parseAnyNumberString();
         // roman, hex, or int
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      void unicodeAppendStr(string& toappend, int thechar); // (private)
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      void expectLabels();
      bool peekingAtLabelStart(); bool paLS() { return peekingAtLabelStart(); }
      bool peekingAtAlphabet();
      bool peekingAtAlnum();
      bool peekingAtAlnumOrUnderscore();
      string peekLabel();
      string peekLabelNoNum();
      string parseLabel();
      shringer parseB64();
      string parseXMLAttribLabel();
      string parseAlphaNumericWord(); // e.g 4chan, 2chan, 2advanced, kira2, medi8or
      string parseNameWord(); // e.g O'Connor, Williams, Kuy'leh. no _ or numbers
      string parseEnglishWord(); // nameword, and ones like e.g i.e A.C.E etc
      string parseHumanLangWord(); // this one isn't limited to english, includes hangul/cyrllic/hebrew support
      string parseSpacedEnglishWords(); // e.g "cottage cheese sandwich" or "prime salad". numbers not supported yet
      string parseSpacedEnglishWordsAndNumbers(bool allow_numbers = true); // e.g "prime-minister number 2"
      string parseAlphaNumericEnglishElement(); // 2advanced.com, will2.0 etc. like englishword, but numeric support
      string parseEnglishElement(char connector1, char connector2, char connector3 = 0);
      string parseCSSClassName();
      string parseLabelNoNum();
      string parseStringLiteralSmart();
      string parseStringLiteralSmartReturnRaw();
      string parseStringLiteral(int thesep = '"');
      bool processBracketContents(char brackettype,  // e.g '('
                                 char equaler,  // e.g '='
                                 char delimiter,  // e.g ','
                                 function<void(const string& dom, const shringer& val)> func);
      string parseLispSymbol();
         // hello"whatsup\n" <-- is a string literal
      string parseJSRegex(bool untileol = true); // javascript regex. may have opener and ender (/ and /)
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      template <class X>
      ptriepath<X> parseTriePath(ptrie<X>& thetrie, bool withaccept = true);
         // gets the furthest valid progerssion
      template <class X>
      ptriepath<X> parseTriePathCaseless(ptrie<X>& thetrie, bool withaccept = true);
         // the trie must be in lowercase mode
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      bool peekingAtWhiteSpaces();
      int skipWhitespaces();
      EasyParser& sw(); // skip whitespaces
      EasyParser& operator--(int); // with even less chars to type. tis is postfix!
      int skipWhitespacesExceptNewline();
      EasyParser& swen();
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // C++ Parsing (kinda best effort, lel)
      int skipCComment(); // skips '//' comment only. will make complicated ones later.
      EasyParser& sc(); // calls the above
      string parseAnsiCComment(); // one /* */ block
      string parseCPlusComment(); // one // block (including newlines)
      string parseCComment(); // one, either of the above. skips whitespaces first too
      string parseCPlusOperand(); // as best as it can do
      string parseCPlusOperandReverse(); // reverse mode
      string parseCPlusUntilEnderOrCommentOrBracketEnd(char theender, char secondender = 0,  char thirdender = 0, char fourthender = 0);
      string parseCPlusUntilSemicolonOrCommentOrBracketEnd();
      string parseCPlusUntilCommaOrCommentOrBracketEnd();
      string parseCPlusUntilBracketEnd(function<bool(int, EasyParser&)> enderfunc = 0);
         // or until enderfunc returns 0. first arg is the next char given
      string parseCPlusTypeString();
      string parseCPlusMethodName(); // e.g WhateverYouWant or operator+ or operator bool
      Shing::CPlusArglist parseCPlusArglistContents(); // e.g "(int x, int yy, const TileIndex&)";
      Shing::CPlusDeclSig parseCPlusDeclSig(); 
         // e.g int SomeClass::getHP(bool blah)"
         // does not parse the = whatever, or function body, or whatever
      shringer parseUntilLineComments(const string& starterpattern);
         // not necessarily C, but handles it just fine for C/C++
         
      string parseCodeSmart(char ender1 = 0, char ender2 = 0, char ender3 = 0, char ender4 = 0);
         // smart because it ends on bracket closer, or if it's a line comment (cpp line comment)
         // also keeps in mind the bracket opens/closes, and string literals
         // actually just calls parseCPlusUntilEnderOrCommentOrBracketEnd
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // comma sets / comma dicts
      // e.g "something [ antialias true, whatever false ]
      // parse the [ and ] openers/closers yourself though
      void parseCommaSet(function<void(const string& thingo)> lefun);
      void parseCommaDict(function<void(const string& var, const string& val)> lefun);
      map<string, string> parseCommaDict();
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      string parseNChars(int nchars);
      string parseNCharsOrUntil(int nchars, int untiler, bool inclusive);
      string parseUntilString(const shringer& towatch, bool inclusive); // not optimized, but meh
      string parseUntil(function<bool(int thechar)> lefunc, bool inclusive);
      string parseUntil(char limiter, bool inclusive);
      string parseUntilChars(string limits, bool inclusive);
      string parseURLElement(bool allowspaces = false); // e.g part of an email address (not including @'s)
      string parseSentenceElement(); // newlines, symbols, or parseAlphaNumericEnglishElement
      string parseUnixFileName(bool allowspaces = true);
      string parseFileName(); // same as parseUnixFileName(false)
      string parseUntilWhitespace(bool inclusive);
      string parseUntilEnd();
      string parseUntilEOL();
      string parseLine(); // basically parseUntil('\n', false) and accept('\n');
      string parseSomeSentenceElement(); // for susie, to parse a sentence element lel
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      EasyParser bs(); // bunshin mode
      // parse and skip. puts bunshinfail if things dont work out
      EasyParser& bsLabel(int count = 1, bool skipspaces = true);
      EasyParser& bsNum(int count = 1, bool skipspaces = true);
      EasyParser& bs(const shringer& tobs);
      EasyParser& bs(char thechar);
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // peekchar test
      bool operator==(int thechar); // bunshin compatible
      // isPeekingAt, really
      bool operator==(const shringer& theword); // bunshin compatible
      // and the negatives
      bool operator!=(int thechar); // bunshin compatible
      bool operator!=(const shringer& theword); // bunshin compatible
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // basically calls peekChar
      int operator[](int64_t theindex);
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // quick accept
      bool operator<<(int thechar); // bunshin compatible
      bool operator<<(const shringer& theword); // bunshin compatible
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      // smart matching
      // (x), (hello) or (13) = puts parse result on the map or vector
      //    it tries to match an english word, or a number, or one char, etc
      //    one day, it may support regex etc!
      // Hello // caps first letter word
      // hello // match this english word
      // "HELLO" // match this string exactly
      // \ // escape
      // space, aka ' ' = skipWhiteSpaces
      // (public)
      bool operator|(const shringer& thecommand); // tries to accept smart match
      bool operator|=(const shringer& thecommand); // tries to check smart match only
      string operator()(const shringer& thevarname);
      string operator()(int thevarindex);
      // (private)
      string& smartMatchResultVecElt(int thenum);
      bool trySmartMatch(const shringer& thecommand);
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      
      // || upgraded expression parsing ||
      EasyExpressionParsage exprParsage();
         // returns an object you can modify for settings
         // run parse() on it to get the EasyExpression
      
      // Expression Parsing! Rudimentary, but good enough for most
      EasyExpression parseExpression(EasyExpressionParsage& expar);
      // EasyExpression parseExpression(EasyExpression startingexp, EasyOperatorPrecedence startingop);
      EasyExpression parseExpressionPrimary(EasyExpressionParsage& expar);
      EasyExpression parseExpression(EasyExpressionParsage& expar, EasyExpression lhs, int minprec);
      string parseExpression_parseBracket(EasyExpressionParsage& expar); // (private)
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
};
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
class BigEndianEasyParser : public EasyParser{
   public:
      BigEndianEasyParser(const shringer& themoo);
      BigEndianEasyParser(const string& themoo, int64_t offset);
      BigEndianEasyParser(const char * thestr, int64_t thelength);
      // BigEndianEasyParser(istream& istr);
};
//////////////////////////////////////////////////////////////////////////////////////////
class UTF8EasyParser : public EasyParser{
   public:
      UTF8EasyParser(const shringer& themoo);
      UTF8EasyParser(const string& themoo, int64_t offset);
      UTF8EasyParser(const char * thestr, int64_t thelength);
      // BigEndianEasyParser(istream& istr);
};
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
template <class X>
ptriepath<X> EasyParser::parseTriePath(ptrie<X>& thetrie, bool withaccept){

   ptriepath<X> lastgood = thetrie.path();
   ptriepath<X> path = lastgood;
   
   EasyParser bunshin = *this;
   
   while(bunshin.hasChar()){
      
      char thechar = bunshin.getChar();
      
      bool succ = path.advance(thechar);
      if(!succ)
         break;
         
      if(path.isvalued()){
         lastgood = path;
         if(withaccept)
            *this = bunshin;
      }
      
   }
   
   return lastgood;
   
   
}
//////////////////////////////////////////////////////////////////////////////////////////
template <class X>
ptriepath<X> EasyParser::parseTriePathCaseless(ptrie<X>& thetrie, bool withaccept){

   ptriepath<X> lastgood = thetrie.path();
   ptriepath<X> path = lastgood;
   
   EasyParser bunshin = *this;
   
   while(bunshin.hasChar()){
      
      char thechar = tolower(bunshin.getChar());
      
      bool succ = path.advance(thechar);
      if(!succ)
         break;
         
      if(path.isvalued()){
         lastgood = path;
         if(withaccept)
            *this = bunshin;
      }
      
   }
   
   return lastgood;
   
   
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////////////////////
