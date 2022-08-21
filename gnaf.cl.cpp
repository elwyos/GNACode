////////////////////////////////////////////////////////////
#include "address.cl.hpp"
#include "../shing/csvlooper.hpp"
#include "../shing/schfile.hpp"
#include "../shing/consolearg.hpp"
#include "../fragdb/db.hpp"
#include "../geo/openlocationcode.hpp"
#include "gnaf.cl.hpp"
#include "gnaf.ceeferinc.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace Gnaf{
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
bool AddressDetail::naiveMatch(int unitnumer, int firstnum, int endnum, int lepostcode, const string& streetphrase){
   if(number_first != firstnum){
      return false;
   }
   if(number_last != endnum){
      return false;
   }
   if(postcode != lepostcode){
      return false;
   }
   if(unitnumer != -1 && unitnumer != flat_number){
      return false;
   }
   string lowered_fullstreet = __toLower(fullStreetName());
   if(Morestring::contains(lowered_fullstreet, streetphrase)){
      return true;
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
int AddressDetail::scoredMatchAssumingMatchedStreet(Address& a){
   int score = 0;
   if(a.lot.valuenum != -1){
      if(__toString(a.lot.valuenum) == lot_number){
         score++;
      }
      if(a.lot.firstvalpostfix == lot_number_suffix){
         score++;
      }
   }
   if(a.unitnumber.postfix == flat_type_code){
      score++;
   }
   if(a.unitnumber.valuenum == flat_number){
      score++;
   }
   if(a.unitnumber.firstvalpostfix == flat_number_suffix){
      score++;
   }
   if(a.floorlevel.postfix == level_type_code){
      score++;
   }
   if(a.floorlevel.valuenum == level_number){
      score++;
   }
   if(a.floorlevel.firstvalpostfix == level_number_suffix){
      score++;
   }
   if(a.propnumber.valuenum == number_first){
      score++;
   }
   if(a.propnumber.firstvalpostfix == number_first_suffix){
      score++;
   }
   if(a.propnumber.secvaluenum == number_last){
      score++;
   }
   if(a.propnumber.secvalpostfix == number_last_suffix){
      score++;
   }
   if(a.locality.value == locality->name){
      score += 5;
   }
   if(postcode == Morestring::toInt(a.postcode.value)){
      score += 10;
   }
   string streetsuffix;
   if(street){
      streetsuffix = street->suffix;
   }
   if(streetsuffix == a.streetname.postfixlong){
      score += 2;
   }
   return score;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
string AddressDetail::fullStreetName(){
   string ret;
   if(street){
      auto& lestr = street.ref();
      ret.append(lestr.name);
      if(lestr.type.size() != 0){
         ret.append(1, ' ');
         ret.append(lestr.type);
      }
      if(lestr.suffix.size() != 0){
         ret.append(1, ' ');
         ret.append(lestr.suffix);
      }
   }
   return ret;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
string AddressDetail::numberStreetPostcodeString(){
   string ret;
   if(number_first != -1){
      ret.append(number_first_prefix);
      ret.append(__toString(number_first));
      ret.append(number_first_suffix);
   }
   if(number_last != -1){
      if(number_first != -1){
         ret.append(1, '-');
      }
      ret.append(number_last_prefix);
      ret.append(__toString(number_last));
      ret.append(number_last_suffix);
   }
   else {
      ret.append(1, ' ');
   }
   if(street){
      auto& lestr = street.ref();
      ret.append(lestr.name);
      if(lestr.type.size() != 0){
         ret.append(1, ' ');
         ret.append(lestr.type);
      }
      if(lestr.suffix.size() != 0){
         ret.append(1, ' ');
         ret.append(lestr.suffix);
      }
   }
   ret.append(1, ' '); // simplemente
   ret.append(__toString(postcode));
   return ret;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool AddressDetail::expired(){
   return !date_retired.isNull();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
string AddressDetail::toSimpleString(){
   string ret;
   if(level_number != -1){
      ret += __toUpper(level_type_code);
      if(level_number_prefix.size() != 0 || level_number_suffix.size() != 0 || level_number != -1){
         ret += " ";
      }
      ret += level_number_prefix;
      if(level_number != -1){
         ret += " " + __toString(level_number);
      }
      ret += level_number_suffix;
      ret += " ";
   }
   if(flat_number != -1){
      ret += __toUpper(flat_type_code);
      ret += " " + string(flat_number_prefix) + __toString(flat_number) + string(flat_number_suffix);
      ret += " ";
   }
   if(number_first != -1){
      ret += number_first_prefix;
      ret += __toString(number_first);
      ret += number_first_suffix;
   }
   if(number_last != -1){
      if(number_first != -1){
         ret += "-";
      }
      ret += number_last_prefix;
      ret += __toString(number_last);
      ret += number_last_suffix;
   }
   else {
      if(number_first != -1){
         ret += " ";
      }
   }
   if(street){
      auto& lestr = street.ref();
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
   }
   else {
      ret += "(no streets?) ";
   }
   if(locality){
      auto& leloc = locality.ref();
      ret += __toLower(leloc.name) + " ";
   }
   ret += __toString(postcode);
   return ret;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
string AddressDetail::plusCode(){
   if(geocode_lat.size() != 0 && geocode_lng.size() != 0){
      double latter = Morestring::toFloat(geocode_lat);
      double longer = Morestring::toFloat(geocode_lng);
      string pluscode = openlocationcode::Encode({latter, longer}, 11); // let's try that much
      return pluscode;
   }
   return "";
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void AddressDetail::print(){
   if(level_number != -1){
      oustoust.otag(__toLower(level_type_code), 0.2f);
      croust>>(240)<<(level_number_prefix )>>(250)<<(level_number)>>(240)<<(level_number_suffix )<<" ";
   }
   if(flat_number != -1){
      oustoust.otag(__toLower(flat_type_code), 0.2f);
      croust>>(69)<<(flat_number_prefix )<<(flat_number )<<(flat_number_suffix )>>(250)<<"/";
   }
   if(number_first != -1){
      croust>>(33)<<(number_first_prefix )>>(250)<<(number_first )>>(33)<<(number_first_suffix )>>(250);
   }
   if(number_last != -1){
      if(number_first != -1){
         croust>>(220)<<"-";
      }
      croust>>(220)<<(number_last_prefix )>>(250)<<(number_last )>>(33)<<(number_last_suffix )>>(250)<<" ";
   }
   else {
      if(number_first != -1){
         croust>>(220)<<" ";
      }
   }
   if(building_name.size() != 0){
      oustoust.otag(__toLower(building_name), 23_xcoel).sym(0x2616, 196_xcoel);
   }
   if(street){
      auto& lestr = street.ref();
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
   }
   else {
      oustoust.tag("!str", 196_xcoel);
   }
   if(location_description.size() != 0){
      oustoust.tag("loc", 112_xcoel);
      croust<<(location_description )<<" ";
   }
   if(private_street.size() != 0){
      oustoust.tag("pri");
      croust<<(private_street )<<" ";
   }
   if(locality){
      auto& leloc = locality.ref();
      oustoust.otag(__toLower(leloc.name)).sym('@');
   }
   oustoust.otag(__toString(postcode), 0.3f);
   string pluscode = plusCode();
   if(pluscode.size() != 0){
      croust>>(33)<<(pluscode )<<" ";
   }
   croust << ozendl;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
Gnaf::Gnaf():
   addies(db.table<AddressDetail>("addies", 12000))
   ,streets(db.table<Street>("streets", 12000))
   ,localities(db.table<Locality>("localities", 12000))
{
   db.setBaseDir("fragilo");
   Frag::push_db(&db);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Gnaf::naiveStreetSearch(const string& phrase){
   EasyParser parsa(phrase);
   int firstnum = parsa--.parseNumber();
   int unitnum = -1;
   if(parsa-- << '/'){
      unitnum = firstnum;
      firstnum = parsa--.parseNumber();
   }
   int secondstreetnum = -1;
   if(parsa-- << '-'){
      secondstreetnum = parsa--.parseNumber();
   }
   string streetname;
   int postcode = -1;
   while(parsa--.hasChar()){
      int peeka = parsa.peekChar();
      if(isdigit(peeka)){
         string numstr = parsa.parseNumberString();
         postcode = Morestring::toInt(numstr);
      }
      else {
         string thingo = parsa.parseSpacedEnglishWords();
      }
   }
   vector<frgLINK<AddressDetail> > matches;
   int matchcount = 0;
   sloopveccounta(addies, moo, counta)
      if(moo->naiveMatch(unitnum, firstnum, secondstreetnum, postcode, streetname)){
         matchcount++;
         matches.push_back(moo);
      }
   sloopend
   sort(matches.begin(), matches.end(), [](auto a, auto b){
      return a->flat_number < b->flat_number;
   });
   sloopvec(matches, moo)
      moo->print();
   sloopend
   oustoust[220].ruler();
   croust>>(33)<<"Matches : ">>(250)<<(matchcount) << ozendl;
   oustoust[220].ruler();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Gnaf::testBuildingMap(){
   int addycount = 0;
   int buildingedcount = 0;
   int expiredcount = 0;
   buildingmap.clear();
   sloopveccounta(addies, moo, counta)
      if(counta % 1000 == 0){
         croust>>(235)<<"... "<<(counta )<<" ..." << ozendl;
      }
      addycount++;
      if(moo->expired()){
         expiredcount++;
      }
      else {
         if(moo->building_name.size() != 0){
            buildingedcount++;
         }
         string buildid = moo->numberStreetPostcodeString();
         auto& lebuild = buildingmap[buildid];
         if(moo->building_name.size() != 0){
            if(lebuild.buildingname.size() != 0){
               if(lebuild.buildingname != (string)moo->building_name){
                  croust>>(196)<<"Building mismatch : ">>(250)<<(lebuild.buildingname )>>(220)<<" vs ">>(250)<<(moo->building_name) << ozendl;
               }
            }
            lebuild.buildingname = moo->building_name; // simple as that
         }
         lebuild.units_inside.push_back(moo);
         auto& leback = lebuild.units_inside.back();
      }
   sloopend
   multimap<int, Building*> biggest_building_order;
   sloopmapex(buildingmap, eh, moo)
      biggest_building_order.insert(make_pair<int, Building*>(-(int)moo.units_inside.size(), &moo));
   sloopend
   oustoust[196].ruler();
   int counta = 0;
   sloopmapex(biggest_building_order, eh, moo)
      int maxlimit = 3;
      oustoust.otag(__toLower(moo->buildingname), 23_xcoel).sym(0x2616, 196_xcoel);
      croust>>(250)<<" @ ">>(220)<<(moo->units_inside.size() )>>(240)<<" units" << ozendl;
      oustoust.push(3);
         sloopveccounta(moo->units_inside, leunit, minicounta)
            if(minicounta > maxlimit){
               croust>>(220)<<"..." << ozendl;
               break;
            }
            leunit->print();
         sloopend
      oustoust.pop();
      counta++;
      if(counta > 10){
         break; // only showing top 10
      }
   sloopend
   oustoust[196].ruler();
   croust>>(33)<<"Addy : ">>(250)<<(addycount) << ozendl;
   croust>>(250)<<"Buildinged : ">>(220)<<(buildingedcount) << ozendl;
   croust>>(196)<<"Expired count : ">>(220)<<(expiredcount) << ozendl;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Gnaf::fill(){
   fill_localities();
   fill_streets();
   fill_addies();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Gnaf::fill_localities(){
   schfile addresses_file(gnaf_psv_folder + gnaf_state_code + "_LOCALITY_psv.psv");
   Shing::CSVLooper looper(addresses_file, true, '|'); // with title, I guess
   int counta = 0;
   oustoust[196].ruler();
   croust>>(33)<<"processing localities.." << ozendl;
   localities.clear(); // clear it all
   while(true){
      auto line = looper.getLine();
      if(!line){
         break;
      }
      else if(!line.is_header){
         counta++;
         if(counta % 1000 == 0 && counta){
            croust>>(33)<<"... ">>(250)<<(counta )>>(33)<<" ..." << ozendl;
         }
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
      }
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
int Gnaf::getLocalityIndex(const string& thename){
   auto it = locality_indexes.find(thename);
   if(it != locality_indexes.end()){
      return it->second;
   }
   return -1;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Gnaf::syncLocalityMap(){
   if(locality_map_synced){
      return;
   }
   sloopvec(localities, moo)
      locality_map[__toLower(moo->name)] = moo;
      locality_trie[__toLower(moo->name)] = moo;
   sloopend
   locality_map_synced = true;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Gnaf::fill_streets(){
   schfile addresses_file(gnaf_psv_folder + gnaf_state_code + "_STREET_LOCALITY_psv.psv");
   Shing::CSVLooper looper(addresses_file, true, '|'); // with title, I guess
   int counta = 0;
   oustoust[196].ruler();
   croust>>(33)<<"processing streets.." << ozendl;
   streets.clear(); // clear it all
   while(true){
      auto line = looper.getLine();
      if(!line){
         break;
      }
      else if(!line.is_header){
         counta++;
         if(counta % 1000 == 0 && counta){
            croust>>(33)<<"... ">>(250)<<(counta )>>(33)<<" ..." << ozendl;
         }
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
      }
   }
   croust<<"Finished with streets.." << ozendl;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
int Gnaf::getStreetIndex(const string& thename){
   auto it = street_indexes.find(thename);
   if(it != street_indexes.end()){
      return it->second;
   }
   return -1;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Gnaf::syncStreetNameMetas(){
   if(streetname_metas_synced){
      return;
   }
   sloopvec(addies, moo)
      if(moo->street){
         auto& lemeta = streetname_metas[__toLower(moo->street->name)];
         lemeta.addies.push_back(moo);
      }
   sloopend
   sloopvec(streets, moo)
      auto& lemeta = streetname_metas[__toLower(moo->name)];
      lemeta.streets.push_back(moo); // damn we need to think about indexing eh
   sloopend
   streetname_metas_synced = true;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
vector<AddressSearchResult> Gnaf::findMatches(Address& addie){
   syncStreetNameMetas();
   vector<AddressSearchResult> results;
   string lookupname = addie.streetname.value;
   if(!streetname_metas.count(lookupname)){
      return results; // no results
   }
   auto& lemeta = streetname_metas[lookupname];
   sloopvec(lemeta.addies, moo)
      int score = moo->scoredMatchAssumingMatchedStreet(addie);
      auto& newres = results.emplace_back();
      newres.address = moo;
      newres.score = score;
   sloopend
   sort(results.begin(), results.end(), [](auto& a, auto& b){
      return a.score > b.score;
   });
   return results;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Gnaf::fill_addies(){
   std::unordered_map<string, pair<string, string> > geocodes;
   {
      schfile geocodes_file(gnaf_psv_folder + gnaf_state_code + "_ADDRESS_DEFAULT_GEOCODE_psv.psv");
      Shing::CSVLooper looper(geocodes_file, true, '|'); // with title, I guess
      int linecounta = 0;
      while(true){
         auto line = looper.getLine();
         if(!line){
            break;
         }
         else if(!line.is_header){
            if(linecounta % 1000 == 0 && linecounta){
               croust>>(33)<<"... ">>(250)<<(linecounta )>>(33)<<" ..." << ozendl;
            }
            string address_detail_pid = line[3];
            string longer = line[5];
            string latter = line[6];
            geocodes[address_detail_pid] = pair<string, string>(latter, longer);
            linecounta++;
         }
      }
   }
   schfile addresses_file(gnaf_psv_folder + gnaf_state_code + "_ADDRESS_DETAIL_psv.psv");
   Shing::CSVLooper looper(addresses_file, true, '|'); // with title, I guess
   int counta = 0;
   oustoust[196].ruler();
   croust>>(220)<<"processing addies.." << ozendl;
   addies.clear(); // clear it all
   while(true){
      auto line = looper.getLine();
      if(!line){
         break;
      }
      else if(!line.is_header){
         counta++;
         if(counta % 1000 == 0 && counta){
            croust>>(33)<<"... ">>(250)<<(counta )>>(33)<<" ..." << ozendl;
         }
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
         n->street.index = getStreetIndex(line[22]);
         n->location_description = line[23];
         n->locality.index = getLocalityIndex(line[24]);
         string aliaser = line[25];
         if(aliaser.size() != 0){
            n->alias_principal = aliaser[0];
         }
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
      }
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
int takeNumberOrNegOne(const string& lestr){
   if(lestr.size() == 0){
      return -1;
   }
   return Morestring::toInt(lestr);
}
int8_t takeOneCharOrNegOne(const string& lestr){
   if(lestr.size() == 0){
      return -1;
   }
   return lestr[0];
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
}; // end namespace Gnaf
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

