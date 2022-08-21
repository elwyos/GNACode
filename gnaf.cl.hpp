////////////////////////////////////////////////////////////
#pragma once
namespace std{}; using namespace std;
////////////////////////////////////////////////////////////
#include "cloth_classdef.cl.hpp"
#include "../shing/shingdate.hpp"
#include "../shing/varchar.hpp"
#include "../fragdb/db.hpp"
#include "../shing/schdangler.hpp"
#include "../shing/ptrie.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace Gnaf{
class Locality{
   public:
   //-------------------------------------------------------
   varCHAR<15> locality_pid;
   SDate date_created;
   SDate date_retired;
   varCHAR<100> name;
   int primary_postcode = -1;
   char locality_class_code = 0;
   varCHAR<15> state;
   varCHAR<15> gnaf_locality_pid;
   int8_t gnaf_reliability_code = -1;
   bool dbp_occupied = false;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class Street{
   public:
   //-------------------------------------------------------
   varCHAR<15> street_pid;
   SDate date_created;
   SDate date_retired;
   char street_class_code = 0;
   varCHAR<100> name;
   varCHAR<15> type;
   varCHAR<15> suffix;
   frgLINK<Locality> locality;
   varCHAR<15> gnaf_street_pid;
   int8_t gnaf_street_confidence = 0;
   int8_t gnaf_reliability_code = 0;
   bool dbp_occupied = false;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class AddressDetail{
   public:
   //-------------------------------------------------------
   varCHAR<15> address_detail_pid;
   SDate date_created = SDate(0);
   SDate date_last_modified = SDate(0);
   SDate date_retired = SDate(0);
   varCHAR<200> building_name;
   varCHAR<2> lot_number_prefix;
   varCHAR<5> lot_number;
   varCHAR<2> lot_number_suffix;
   varCHAR<7> flat_type_code;
   varCHAR<2> flat_number_prefix;
   int32_t flat_number = -1;
   varCHAR<2> flat_number_suffix;
   varCHAR<4> level_type_code;
   varCHAR<2> level_number_prefix;
   int32_t level_number = -1;
   varCHAR<2> level_number_suffix;
   varCHAR<3> number_first_prefix;
   int32_t number_first = -1;
   varCHAR<2> number_first_suffix;
   varCHAR<3> number_last_prefix;
   int32_t number_last = -1;
   varCHAR<2> number_last_suffix;
   frgLINK<Street> street;
   varCHAR<45> location_description;
   frgLINK<Locality> locality;
   char alias_principal = 0;
   int32_t postcode = -1;
   varCHAR<75> private_street;
   varCHAR<20> legal_parcel_id;
   int8_t confidence = 0;
   varCHAR<15> address_site_pid;
   int8_t level_geocoded_code = 0;
   varCHAR<15> property_pid;
   varCHAR<15> gnaf_property_pid;
   char primary_secondary = 0;
   varCHAR<22> geocode_lat;
   varCHAR<22> geocode_lng;
   bool dbp_occupied = false;
   //-------------------------------------------------------
   bool naiveMatch(int unitnumer, int firstnum, int endnum, int lepostcode, const string& streetphrase);
   int scoredMatchAssumingMatchedStreet(Address& a);
   string fullStreetName();
   string numberStreetPostcodeString();
   bool expired();
   string toSimpleString();
   string plusCode();
   void print();
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class Building{
   public:
   //-------------------------------------------------------
   string buildingname;
   vector<frgLINK<AddressDetail>> units_inside;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class AddressSearchResult{
   public:
   //-------------------------------------------------------
   Frag::PREF<AddressDetail> address;
   int score = 0;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class StreetNameMeta{
   public:
   //-------------------------------------------------------
   vector<Frag::PREF<Street>> streets;
   vector<Frag::PREF<AddressDetail>> addies;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class Gnaf{
   public:
   //-------------------------------------------------------
   string gnaf_psv_folder = "/home/yose/dev/geo/gnafpsvs/";
   string gnaf_state_code = "VIC";
   frgDB db;
   frgVEC<Locality>& localities;
   frgVEC<Street>& streets;
   frgVEC<AddressDetail>& addies;
   map<string,Building> buildingmap;
   map<string,int> locality_indexes;
   map<string,int> street_indexes;
   bool locality_map_synced = false;
   map<string,Frag::PREF<Locality>> locality_map;
   ptrie<Frag::PREF<Locality>> locality_trie;
   bool streetname_metas_synced = false;
   map<string,StreetNameMeta> streetname_metas;
   //-------------------------------------------------------
   Gnaf();
   void naiveStreetSearch(const string& phrase);
   void testBuildingMap();
   void fill();
   void fill_localities();
   int getLocalityIndex(const string& thename);
   void syncLocalityMap();
   void fill_streets();
   int getStreetIndex(const string& thename);
   void syncStreetNameMetas();
   vector<AddressSearchResult> findMatches(Address& addie);
   void fill_addies();
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
int takeNumberOrNegOne(const string& lestr);
////////////////////////////////////////////////////////////
int8_t takeOneCharOrNegOne(const string& lestr);
////////////////////////////////////////////////////////////
DECLschdangler(gnaf, Gnaf);
}; // end namespace Gnaf
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

