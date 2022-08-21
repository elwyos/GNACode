//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#bodinc gnaf neocontext shing.strext
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/AddieParser(const shringer& leshring)
   :offiparsa(leshring)
   
   offiparsa.caseless();
   
   performParse();
   
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/performParse()
   
   // so first, try get the street numbering eh?
   
   bool phase15_reached = false;
   bool phase2_reached = false;
   bool phase3_reached = false;
   
   // basically.. set up a bunch of parsers
   \while offiparsa--.hasChar()
      
      // this one is independent
      // forget po boxes for now
      // parseDeliveryType();
      
      bool gotit = false; // did it get anything on this pass?
      
      // -------- phase 1 ----------------
      \if !phase15_reached
         \if !gotit && !ret.unitnumber
            gotit = tryParseUnitNumber();
            
         \if !gotit && !ret.floorlevel
            gotit = tryParseFloorLevel();
         
      // -------- phase 1.5 kinda ----------------
         
      \if !gotit && !ret.propnumber
         gotit = tryParseHousePropertyNumber();
         \if gotit
            phase15_reached = true;
         
      \if !gotit && !ret.lot
         gotit = tryParseLot();
         \if gotit
            phase15_reached = true;
      
      // -------- phase 2 ----------------
      \if !gotit && (ret.propnumber || ret.lot) && !ret.streetname
         gotit = parseStreetNameAndType();
         \if gotit
            phase2_reached = true;
      
      // -------- phase 3 ----------------
      \if !gotit && ret.streetname
      
         \if !gotit && !ret.locality
            gotit = parseLocality();
            \if gotit
               phase3_reached = true;
            
         \if !gotit && !ret.state
            gotit = parseStateTerritory();
            \if gotit
               phase3_reached = true;
            
         \if !gotit && !ret.postcode
            gotit = parsePostCode();
            \if gotit
               phase3_reached = true;
      
      // forget country for now
      \if gotit
         \if offiparsa-- << ','
            // cool, can accept it
            
      \else
         break; // nope, we failed...
         
   // here here
   
   
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/maybeParseCodedWords(EasyParser& parsa, const vector<pair<string, string>>& codes) -> tuple<string, string, bool>
   
   parsa--;
   \sloop codes
      \if parsa << moo.first
         return tuple<string, string, bool>(moo.first, moo.second, false);
      \if parsa << moo.second
         \if(parsa << '.')
            // cool, maybe they add a dot afterwards
         return tuple<string, string, bool>(moo.first, moo.second, true);
   
   return tuple<string, string, bool>(string(), string(), false);
   
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/parseEsotericNumber(EasyParser& _parsa, AddieCompo& ret) -> bool
   EasyParser parsa = _parsa;
   
   // special exception for letters
   \scope
      EasyParser tempa = _parsa;
      string wholelabel = tempa--.parseLabel();
      \if wholelabel.size() == 1
         ret.firstvalpostfix = wholelabel;
         _parsa = tempa;
         return true;
   
   \if !parsa--.paNUM()
      return false;
   
   ret.valuenum = parsa.parseNumber();
   
   \if parsa.peekingAtAlphabet()
      ret.firstvalpostfix = string(1, parsa.getChar());
   
   EasyParser beforeconnector = parsa;
   
   gplaicht(parsing_ranged_address);
   \if parsa-- << '-' && parsing_ranged_address
      // got connector
      ret.connector = '-';
   \else
      // well, we got the soteric number
      _parsa = parsa;
      return true;
      
   \if !parsa--.paNUM()
      ret.connector = 0;
      _parsa = beforeconnector;
      return true;
      
   ret.secvaluenum = parsa--.parseNumber();
   \if parsa.peekingAtAlphabet()
      ret.secvalpostfix = string(1, parsa.getChar());
   
   _parsa = parsa;
   // cool
   return true;
   
   // B32-44num
   
   
   
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/tryParseUnitNumber() -> bool
   
   EasyParser parsa = offiparsa;
   
   \scope
      // test unit number with slash notation
      EasyParser tempa = offiparsa;
      AddieCompo compo;
      
      \if parseEsotericNumber(tempa, compo)
         // well, we got the esoteric number
         \if tempa-- << '/'
            // well, that's the unit number then
            compo.postfixlong = "unit";
            compo.postfix = "u";
            
            offiparsa = tempa;
            return true;
      
   
   
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
   
   \if parsa--.paLS()
      auto [longname, shortened, abbreved] = maybeParseCodedWords(parsa, validcodes);
      compo.postfixlong = longname;
      compo.postfix = shortened;
      
      \if shortened == "f" || shortened == "apt"
         compo.postfixlong = "unit";
         compo.postfix = "u"; // interchangeable, so..
      
      \if longname.size() != 0
         // good, we got it
         \if parseEsotericNumber(parsa, compo)
            ret.unitnumber = compo;
            ret.unitnumber.filled = true;
            offiparsa = parsa;
            return true;
   
   return false;
   
   
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/tryParseFloorLevel() -> bool

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
   
   \if parsa--.paLS()
      auto [longname, shortened, abbreved] = maybeParseCodedWords(parsa, validcodes);
      compo.postfixlong = longname;
      compo.postfix = shortened;
      
      \if longname.size() != 0
         // good, we got it
         // need number?
         \if parsa-- << ","
            // no numbers necessary. e.g first ground floor,
         \if shortened == "fl" || shortened == "l"
            // we need numbers
            compo.postfixlong = "level";
            compo.postfix = "l";
            // because floor and level is interchangeable
            \if parseEsotericNumber(parsa, compo)
               ret.floorlevel = compo;
               ret.floorlevel.filled = true;
               offiparsa = parsa;
               return true;
            \else
               return false; // we need a number bro
         
         offiparsa = parsa;
         return true; // no first component
         
   return false;
   
   // level
   // basement, floor, ground floor, lower ground floor, mezzanine, upper ground floor
   // b, fl, g, l, lg, m, ug
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/tryParseHousePropertyNumber() -> bool

   EasyParser parsa = offiparsa;
      
   AddieCompo compo;
   
   splaicht(parsing_ranged_address);
   \if parseEsotericNumber(parsa, compo)
      ret.propnumber = compo;
      ret.propnumber.filled = true;
      offiparsa = parsa;
      return true;
   \else
      return false; // we need a number bro
      
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/tryParseLot() -> bool
   // lot 17, lot 16, whatever
   
   EasyParser parsa = offiparsa;
   
   \if parsa-- << "lot"
      AddieCompo compo;
      \if parseEsotericNumber(parsa, compo)
         compo.postfixlong = "lot";
         compo.postfix = "lot";
         ret.lot = compo;
         ret.lot.filled = true;
         offiparsa = parsa;
         return true;
      
   return false;
   
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/parseStreetNameAndType() -> bool
   // essential lol
   
   // alley ally
   // grove gr
   // arcade arc
   // highway hwy
   // avenue ave
   // lane lane
   // boulevard bvd
   // parade pde
   // close cl
   // place pl
   // court ct
   // road rd
   // crescent cres
   // square sq
   // drive dr
   // street st
   // esplanade esp
   // terrace tce
   
   static vector<pair<string, string> > validcodes;
   \once
      Strext lestrext;
      lestrext.loadFile("bundle/street_abbrevs.strext");
      \sloop lestrext
         vector<string> words;
         __splitOn(moo.tag, ' ', words);
         \if words.size() >= 2
            string abbrever = __toLower(words.back());
            words.pop_back();
            string longform = __toLower(Morestring::unsplit(words, ' '));
            validcodes.push_back(pair<string, string>(longform, abbrever));
   
   EasyParser parsa = offiparsa;
   
   string name_buildup;
   \while parsa--.paLS()
      \if name_buildup.size() != 0
         name_buildup.append(1, ' ');
      name_buildup.append(__toLower(parsa.parseLabel()));
      
      auto [longname, shortened, abbreved] = maybeParseCodedWords(parsa, validcodes);
      
      \if shortened.size() != 0
         // got the closer
         AddieCompo compo;
         compo.value = name_buildup;
         compo.postfixlong = longname;
         compo.postfix = shortened;
         
         ret.streetname = compo;
         ret.streetname.filled = true;
         offiparsa = parsa;
         return true;

   return false;

      
   
   
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/parseDeliveryType() -> bool
   return false;
   // could be a whole address by itself
   // actually ignore this for now, because GNAF doesn't have post office data
   
   // Care of Post Office = CARE PO
   // Community Mail Bag / CMB
   // General Post Office Box / GPO BOX
   // Mail Service / MS
   // Roadside Delivery / RSD
   // Roadside Mail Service / RMS
   // Community Mail Agent / CMA
   // Community Postal Agent / CPA
   // Locked Bag / LOCKED BAG
   // Post Office Box / PO BOX
   // Roadside Mail Box/Bag = RMB
   // Private Bag / PRIVATE BAG
   
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/parseLocality() -> bool
   // suburb, so.. look from a list first from gnaf
   // get the suburbs list
   
   EasyParser parsa = offiparsa;
   
   gnaf->syncLocalityMap();
   
   auto lepath = parsa.parseTriePathCaseless(gnaf->locality_trie);
   
   
   \if lepath.isvalued()
      // well
      string localityname = __toLower(lepath.getval()->name);
      ret.locality.value = localityname; // no postifix
      ret.locality.filled = true;
      offiparsa = parsa;
      return true;
      
   return false;
      
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/parseStateTerritory() -> bool

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
   
   \if shortened.size() != 0
      
      ret.state.value = shortened; // just use the short state code
      ret.state.filled = true;
      
      offiparsa = parsa;
      return true;
      
   
   return false;
   
   
   // there's only a few
   // Australian Capital Territory / ACT
   // New South Wales / NSW
   // Northern Territory / NT
   // ...
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/parsePostCode() -> bool
   // four digits
   EasyParser parsa = offiparsa;
   
   string numstr = parsa--.parseNumberString();
   \if numstr.size() == 4
      // cool
      ret.postcode.value = numstr;
      ret.postcode.filled = true;
      offiparsa = parsa;
      return true;
   
   return false;
   
//////////////////////////////////////////////////////////////////////////////////////////
AddieParser/parseCountry() -> bool
   // lol, australia only for now
   return false;
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
