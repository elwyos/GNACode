//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#bodinc csvlooper schfile shing.consolearg
//////////////////////////////////////////////////////////////////////////////////////////
#bodinc fragdb.db geo.openlocationcode
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
AddressDetail/naiveMatch(int unitnumer, int firstnum, int endnum, int lepostcode, const string& streetphrase) -> bool
   \if number_first != firstnum
      return false;
      
   \if number_last != endnum
      return false;
      
   \if postcode != lepostcode
      return false;
      
   \if unitnumer != -1 && unitnumer != flat_number
      return false;
   
   string lowered_fullstreet = __toLower(fullStreetName());
   \if Morestring::contains(lowered_fullstreet, streetphrase)
      return true;
   return false;
//////////////////////////////////////////////////////////////////////////////////////////
AddressDetail/scoredMatchAssumingMatchedStreet(Address& a) -> int
   
   // well, we assume street name matches here
   int score = 0;
   
   \if a.lot.valuenum != -1
      \if __toString(a.lot.valuenum) == lot_number
         score++;
      \if a.lot.firstvalpostfix == lot_number_suffix
         score++;
   
   \if a.unitnumber.postfix == flat_type_code
      score++;
   \if a.unitnumber.valuenum == flat_number
      score++;
   \if a.unitnumber.firstvalpostfix == flat_number_suffix
      score++;
      
   \if a.floorlevel.postfix == level_type_code
      score++;
   \if a.floorlevel.valuenum == level_number
      score++;
   \if a.floorlevel.firstvalpostfix == level_number_suffix
      score++;
      
   \if a.propnumber.valuenum == number_first
      score++;
   \if a.propnumber.firstvalpostfix == number_first_suffix
      score++;
      
   \if a.propnumber.secvaluenum == number_last
      score++;
   \if a.propnumber.secvalpostfix == number_last_suffix
      score++;
      
   \if a.locality.value == locality->name
      score += 5;
      
   \if postcode == Morestring::toInt(a.postcode.value)
      score += 10;
   
   string streetsuffix;
   
   \if street
      streetsuffix = street->suffix;
   
   \if streetsuffix == a.streetname.postfixlong
      score += 2;
      
   
   return score;
//////////////////////////////////////////////////////////////////////////////////////////
AddressDetail/fullStreetName() -> string
   string ret;
   \if street
      auto& lestr = street.ref();
      // well we assume it exists..
      ret.append(lestr.name);
      if(lestr.type.size() != 0){
         ret.append(1, ' ');
         ret.append(lestr.type);
      }
      
      if(lestr.suffix.size() != 0){
         ret.append(1, ' ');
         ret.append(lestr.suffix);
      }
   
   return ret;
//////////////////////////////////////////////////////////////////////////////////////////
AddressDetail/numberStreetPostcodeString() -> string
   
   string ret;
   
   \if number_first != -1
      ret.append(number_first_prefix);
      ret.append(__toString(number_first));
      ret.append(number_first_suffix);
   
   \if number_last != -1
      \if number_first != -1
         ret.append(1, '-');
      
      ret.append(number_last_prefix);
      ret.append(__toString(number_last));
      ret.append(number_last_suffix);
   \else
      ret.append(1, ' ');
      
   \if street
      auto& lestr = street.ref();
      // well we assume it exists..
      ret.append(lestr.name);
      if(lestr.type.size() != 0){
         ret.append(1, ' ');
         ret.append(lestr.type);
      }
      
      if(lestr.suffix.size() != 0){
         ret.append(1, ' ');
         ret.append(lestr.suffix);
      }
   
   ret.append(1, ' '); // simplemente
   ret.append(__toString(postcode));
   
   return ret;
   
   
//////////////////////////////////////////////////////////////////////////////////////////
AddressDetail/expired() -> bool
   return !date_retired.isNull();
//////////////////////////////////////////////////////////////////////////////////////////
AddressDetail/toSimpleString() -> string
   
   string ret;
   
   \if level_number != -1
      //oustoust.printTag(level_type_code, 240_xcoel);
      ret += __toUpper(level_type_code);
      
      \if level_number_prefix.size() != 0 || level_number_suffix.size() != 0 || level_number != -1
         ret += " ";
         
      ret += level_number_prefix;
      \if level_number != -1
         ret += " " + __toString(level_number);
      ret += level_number_suffix;
      
      ret += " ";
      
   
   \if flat_number != -1
      //oustoust.printTag(flat_type_code, 240_xcoel);
      ret += __toUpper(flat_type_code);
      ret += " " + string(flat_number_prefix) + __toString(flat_number) + string(flat_number_suffix);
      ret += " ";
      
   \if number_first != -1
      ret += number_first_prefix;
      ret += __toString(number_first);
      ret += number_first_suffix;
      
   
      
   \if number_last != -1
      \if number_first != -1
         ret += "-";
      ret += number_last_prefix;
      ret += __toString(number_last);
      ret += number_last_suffix;
   \else
      \if number_first != -1
         ret += " ";
   
   
   \if street
      auto& lestr = street.ref();
      // well we assume it exists..
      string strfull = lestr.name;
      if(lestr.type.size() != 0){
         strfull.append(1, ' ');
         strfull.append(lestr.type);
      }
      
      if(lestr.suffix.size() != 0){
         strfull.append(1, ' ');
         strfull.append(lestr.suffix);
      }
      
      ret += strfull + " ";
   \else
      // no street..
      ret += "(no streets?) ";

   /*
   \if location_description.size() != 0
      oustoust.tag("loc", 112_xcoel);
      \\/ location_description " " \
      
   \if private_street.size() != 0
      oustoust.tag("pri");
      \\/ private_street " " \
   */
   
   \if locality
      auto& leloc = locality.ref();
      ret += __toLower(leloc.name) + " ";
      
   ret += __toString(postcode);
   
   return ret;

//////////////////////////////////////////////////////////////////////////////////////////
AddressDetail/plusCode() -> string
   \if geocode_lat.size() != 0 && geocode_lng.size() != 0
      
      double latter = Morestring::toFloat(geocode_lat);
      double longer = Morestring::toFloat(geocode_lng);
      
      string pluscode = openlocationcode::Encode({latter, longer}, 11); // let's try that much
      return pluscode;
   
   return "";
//////////////////////////////////////////////////////////////////////////////////////////
AddressDetail/print()
   //\\/ [240] address_detail_pid [240] " : " \
   
   // try to print the shit
   
   \if level_number != -1
      //oustoust.printTag(level_type_code, 240_xcoel);
      oustoust.otag(__toLower(level_type_code), 0.2f);
      \\/ [240] level_number_prefix [250]level_number[240]level_number_suffix " " \
   
   \if flat_number != -1
      //oustoust.printTag(flat_type_code, 240_xcoel);
      oustoust.otag(__toLower(flat_type_code), 0.2f);
      \\/ [69] flat_number_prefix flat_number flat_number_suffix [250]"/" \
      
   \if number_first != -1
      \\/ [33] number_first_prefix [250]number_first [33]number_first_suffix [250] \
      
   \if number_last != -1
      \if number_first != -1
         \\/ [220] "-" \
      \\/ [220] number_last_prefix [250]number_last [33]number_last_suffix [250]" " \
   \else
      \if number_first != -1
         \\/ [220] " " \ 
   
      
   \if building_name.size() != 0
      oustoust.otag(__toLower(building_name), 23_xcoel).sym(0x2616, 196_xcoel);
      //oustoust.printTagI(0x2616, 220_xcoel, __toLower(building_name), 240_xcoel);
      //\\/ [55] building_name " " \
   
   
   \if street
      auto& lestr = street.ref();
      // well we assume it exists..
      string strfull = lestr.name;
      if(lestr.type.size() != 0){
         strfull.append(1, ' ');
         strfull.append(lestr.type);
      }
      
      if(lestr.suffix.size() != 0){
         strfull.append(1, ' ');
         strfull.append(lestr.suffix);
      }
      
      oustoust.otag(__toLower(strfull), 0.2f).slim().sym(0x2550, 220_xcoel).endsym(0x2550, 220_xcoel);
   \else
      oustoust.tag("!str", 196_xcoel);

   \if location_description.size() != 0
      oustoust.tag("loc", 112_xcoel);
      \\/ location_description " " \
      
   \if private_street.size() != 0
      oustoust.tag("pri");
      \\/ private_street " " \
      
   
   \if locality
      auto& leloc = locality.ref();
      oustoust.otag(__toLower(leloc.name)).sym('@');
      
   oustoust.otag(__toString(postcode), 0.3f);
   
   
   string pluscode = plusCode();
   \if pluscode.size() != 0
      \\/ [33] pluscode " " \
   
   
   \\/
   // finish the line
   
   
   /*
   
   oustoust.push(3);
      
      \\/ [240] "created " [250] date_created.iso8601DateString() \
      \if date_last_modified
         \\/ [240] " modified " [22] date_last_modified.iso8601DateString() \
      \if date_retired
         oustoust.otag("retired " + date_retired.iso8601DateString(), 196_xcoel).shim(220_xcoel);
      \\/
      
   oustoust.pop();
   */
   
   
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/Gnaf()
   :addies(db.table<AddressDetail>("addies", 12000))
   :streets(db.table<Street>("streets", 12000))
   :localities(db.table<Locality>("localities", 12000))
   
   // yeah the boys!
   db.setBaseDir("fragilo");
   
   Frag::push_db(&db);
   // just use this one
   
   
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/naiveStreetSearch(const string& phrase)
   EasyParser parsa(phrase);
   
   int firstnum = parsa--.parseNumber();
   
   int unitnum = -1;
   
   \if parsa-- << '/'
      unitnum = firstnum;
      firstnum = parsa--.parseNumber();
   
   int secondstreetnum = -1;
   \if parsa-- << '-'
      secondstreetnum = parsa--.parseNumber();
      
   string streetname;
   
   int postcode = -1;
   
   \while parsa--.hasChar()
      int peeka = parsa.peekChar();
      \if isdigit(peeka)
         string numstr = parsa.parseNumberString();
         postcode = Morestring::toInt(numstr);
      \else
         string thingo = parsa.parseSpacedEnglishWords();
         
   vector<frgLINK<AddressDetail> > matches;
   int matchcount = 0;
   \sloop addies, moo, counta
      \if moo->naiveMatch(unitnum, firstnum, secondstreetnum, postcode, streetname)
         matchcount++;
         matches.push_back(moo);
         //matches.push_back(frgLINK<AddressDetail>());
         //matches.back().index = counta;
         //moo->print();
         
   sort(matches.begin(), matches.end(), [](auto a, auto b){
      return a->flat_number < b->flat_number;
   });
   
   \sloop matches
      moo->print();
   
   oustoust[220].ruler();
   \\/ [33] "Matches : " [250] matchcount
   oustoust[220].ruler();
   
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/testBuildingMap()
   
   int addycount = 0;
   int buildingedcount = 0;
   int expiredcount = 0;
   
   buildingmap.clear();
   
   \sloop addies, moo, counta
      \if counta % 1000 == 0
         \\/ [235] "... " counta " ..."
      
      addycount++;
      
      \if moo->expired()
         expiredcount++;
      \else
      
         \if moo->building_name.size() != 0
            buildingedcount++;
            
            // anyways, let's try this first. maybe some of them don't have names
         string buildid = moo->numberStreetPostcodeString();
         auto& lebuild = buildingmap[buildid];
         \if moo->building_name.size() != 0
            \if lebuild.buildingname.size() != 0
               \if lebuild.buildingname != (string)moo->building_name
                  \\/ [196] "Building mismatch : " [250] lebuild.buildingname [220] " vs " [250] moo->building_name
            lebuild.buildingname = moo->building_name; // simple as that
         
         lebuild.units_inside.push_back(moo);
         auto& leback = lebuild.units_inside.back();
      
   
   multimap<int, Building*> biggest_building_order;
   \sloopmap buildingmap
      biggest_building_order.insert(make_pair<int, Building*>(-(int)moo.units_inside.size(), &moo));
      
      
   oustoust[196].ruler();
      
   int counta = 0;
   \sloopmap biggest_building_order
   
      int maxlimit = 3;
   
      oustoust.otag(__toLower(moo->buildingname), 23_xcoel).sym(0x2616, 196_xcoel);
      \\/ [250] " @ " [220] moo->units_inside.size() [240] " units"
      oustoust.push(3);
         \sloopvec moo->units_inside, leunit, minicounta
            \if minicounta > maxlimit
               \\/[220] "..."
               break;
            leunit->print();
      oustoust.pop();
      
      counta++;
      
      \if counta > 10
         break; // only showing top 10
      
   oustoust[196].ruler();
   
         
   \\/ [33] "Addy : " [250] addycount
   \\/ [250] "Buildinged : " [220] buildingedcount
   \\/ [196] "Expired count : " [220] expiredcount
   
   
   
//////////////////////////////////////////////////////////////////////////////////////////
++/takeNumberOrNegOne(const string& lestr) -> int
   \if lestr.size() == 0
      return -1;
   
   return Morestring::toInt(lestr);
//////////////////////////////////////////////////////////////////////////////////////////
++/takeOneCharOrNegOne(const string& lestr) -> int8_t
   \if lestr.size() == 0
      return -1;
   
   return lestr[0];
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/fill()
   
   fill_localities();
   fill_streets();
   fill_addies();
   
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/fill_localities()
   
   schfile addresses_file(gnaf_psv_folder + gnaf_state_code + "_LOCALITY_psv.psv");
   Shing::CSVLooper looper(addresses_file, true, '|'); // with title, I guess
   // but wait..
   
   int counta = 0;
   
   oustoust[196].ruler();
   \\/ [33] "processing localities.."
   
   localities.clear(); // clear it all
   \while true
      auto line = looper.getLine();
      \if !line
         break;
      \elif !line.is_header
         counta++;
         \if counta % 1000 == 0 && counta
            \\/ [33] "... " [250] counta [33] " ..."
         
         auto r = localities.make();
         r->locality_pid = line[0];
         r->date_created = SDate::parseSmartish(line[1], 'd'); // lol day first please
         r->date_retired = SDate::parseSmartish(line[2], 'd'); // lol day first please
         r->name = line[3];
         r->primary_postcode = takeNumberOrNegOne(line[4]);
         r->locality_class_code = takeOneCharOrNegOne(line[5]);
         r->state = line[6];
         r->gnaf_locality_pid = line[7];
         r->gnaf_reliability_code = takeOneCharOrNegOne(line[8]);
         
         r.save();
         
         locality_indexes[line[0]] = r.index; // get them index yeah
         // hmm, msync should be called later I guess, but we'll try this first
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/getLocalityIndex(const string& thename) -> int
   auto it = locality_indexes.find(thename);
   \if it != locality_indexes.end()
      return it->second;
   
   return -1;
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/syncLocalityMap()
   \if locality_map_synced
      return;
   
   \sloop localities
      locality_map[__toLower(moo->name)] = moo;
      // yepge
      locality_trie[__toLower(moo->name)] = moo;
   
   locality_map_synced = true;
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/fill_streets()
   
   schfile addresses_file(gnaf_psv_folder + gnaf_state_code + "_STREET_LOCALITY_psv.psv");
   Shing::CSVLooper looper(addresses_file, true, '|'); // with title, I guess
   // but wait..
   
   int counta = 0;
   
   oustoust[196].ruler();
   \\/ [33] "processing streets.."
   
   
   streets.clear(); // clear it all
   \while true
      auto line = looper.getLine();
      \if !line
         break;
      \elif !line.is_header
         counta++;
         
         \if counta % 1000 == 0 && counta
            \\/ [33] "... " [250] counta [33] " ..."
         
         auto r = streets.make();
         r->street_pid = line[0];
         r->date_created = SDate::parseSmartish(line[1], 'd'); // lol day first please
         r->date_retired = SDate::parseSmartish(line[2], 'd'); // lol day first please
         r->street_class_code = takeOneCharOrNegOne(line[3]);
         r->name = line[4];
         r->type = line[5];
         r->suffix = line[6];
         r->locality.index = getLocalityIndex(line[7]);
         
         r->gnaf_street_pid = line[8];
         r->gnaf_street_confidence = takeNumberOrNegOne(line[9]);
         r->gnaf_reliability_code = takeNumberOrNegOne(line[10]);
         
         r.save();
         
         street_indexes[line[0]] = r.index; // get them index yeah
         // hmm, msync should be called later I guess, but we'll try this first
   
   \\/ "Finished with streets.."
   
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/getStreetIndex(const string& thename) -> int
   auto it = street_indexes.find(thename);
   \if it != street_indexes.end()
      return it->second;
   
   return -1;
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/syncStreetNameMetas()
   \if streetname_metas_synced
      return;
   
   \sloop addies
      \if moo->street
         auto& lemeta = streetname_metas[__toLower(moo->street->name)];
         lemeta.addies.push_back(moo);
         
   \sloop streets
      auto& lemeta = streetname_metas[__toLower(moo->name)];
      lemeta.streets.push_back(moo); // damn we need to think about indexing eh
      
   
   
   streetname_metas_synced = true;
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/findMatches(Address& addie) -> vector<AddressSearchResult>
   
   syncStreetNameMetas();
   
   vector<AddressSearchResult> results;
   
   // well
   string lookupname = addie.streetname.value;
   
   \if !streetname_metas.count(lookupname)
      return results; // no results
   
   auto& lemeta = streetname_metas[lookupname];
   \sloop lemeta.addies
      int score = moo->scoredMatchAssumingMatchedStreet(addie);
      
      auto& newres = results.emplace_back();
      newres.address = moo;
      newres.score = score;
      
   sort(results.begin(), results.end(), [](auto& a, auto& b){
      return a.score > b.score;
   });
   
   return results;
   
//////////////////////////////////////////////////////////////////////////////////////////
Gnaf/fill_addies()

   // default geocodes
   std::unordered_map<string, pair<string, string> > geocodes;
      // in latlong thanks
   \scope
      schfile geocodes_file(gnaf_psv_folder + gnaf_state_code + "_ADDRESS_DEFAULT_GEOCODE_psv.psv");
      Shing::CSVLooper looper(geocodes_file, true, '|'); // with title, I guess
      
      int linecounta = 0;
      
      \while true
         auto line = looper.getLine();
         \if !line
            break;
         \elif !line.is_header
         
            \if linecounta % 1000 == 0 && linecounta
               \\/ [33] "... " [250] linecounta [33] " ..."
         
            string address_detail_pid = line[3];
            string longer = line[5];
            string latter = line[6];
            
            
            geocodes[address_detail_pid] = pair<string, string>(latter, longer);
            linecounta++;
      
   schfile addresses_file(gnaf_psv_folder + gnaf_state_code + "_ADDRESS_DETAIL_psv.psv");
   Shing::CSVLooper looper(addresses_file, true, '|'); // with title, I guess
   // but wait..
   
   int counta = 0;
   
   oustoust[196].ruler();
   \\/ [220] "processing addies.."
   
   addies.clear(); // clear it all
   \while true
      auto line = looper.getLine();
      \if !line
         break;
      \elif !line.is_header
         counta++;
         
         \if counta % 1000 == 0 && counta
            \\/ [33] "... " [250] counta [33] " ..."
         
         auto n = addies.make();
         n->address_detail_pid = line[0];
         
         pair<string, string> latlonger = geocodes[line[0]];
         n->geocode_lat = latlonger.first;
         n->geocode_lng = latlonger.second;
         
         n->date_created = SDate::parseSmartish(line[1], 'd'); // lol day first please
         n->date_last_modified = SDate::parseSmartish(line[2], 'd'); // lol day first please
         n->date_retired = SDate::parseSmartish(line[3], 'd'); // lol day first please
         n->building_name = line[4];
         
         n->lot_number_prefix = line[5];
         n->lot_number = line[6];
         n->lot_number_suffix = line[7];
         
         n->flat_type_code = line[8];
         n->flat_number_prefix = line[9];
         string flatnumberer = line[10];
         n->flat_number = takeNumberOrNegOne(line[10]);
         
         n->flat_number_suffix = line[11];
         
         n->level_type_code = line[12];
         n->level_number_prefix = line[13];
         n->level_number = takeNumberOrNegOne(line[14]);
         n->level_number_suffix = line[15];
         
         n->number_first_prefix = line[16];
         n->number_first = takeNumberOrNegOne(line[17]);
         n->number_first_suffix = line[18];
         
         n->number_last_prefix = line[19];
         n->number_last = takeNumberOrNegOne(line[20]);
         n->number_last_suffix = line[21];
         
         //n->street_locality_pid = line[22];
         n->street.index = getStreetIndex(line[22]);
         
         n->location_description = line[23];
         
         //n->locality_pid = line[24];
         n->locality.index = getLocalityIndex(line[24]);
         
         string aliaser = line[25];
         \if aliaser.size() != 0
            n->alias_principal = aliaser[0];
         n->postcode = takeNumberOrNegOne(line[26]);
         
         n->private_street = line[27];
         n->legal_parcel_id = line[28];
         n->confidence = Morestring::toInt(line[29]);
         
         n->address_site_pid = line[30];
         n->level_geocoded_code = Morestring::toInt(line[31]);
         
         n->property_pid = line[32];
         n->gnaf_property_pid = line[32];
         
         string primsec = line[34];
         if(primsec.size() != 0){
            n->primary_secondary = primsec[0];
         }
         
         n.save();
         
         /*
         \if intrandbetween(0, 500) == 0
            line.print();
            n->print();
         */
         // hmm, msync should be called later I guess, but we'll try this first
   
   

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
