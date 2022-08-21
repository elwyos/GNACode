////////////////////////////////////////////////////////////
#pragma once
namespace std{}; using namespace std;
////////////////////////////////////////////////////////////
#include "cloth_classdef.cl.hpp"
#include "../shing/schdangler.hpp"
#include "../servo/servo.hpp"
#include "../shing/flector.hpp"
#include "../servo/routespecifier.hpp"
#include "../servo/connection.hpp"
#include "../servo/request.hpp"
#include "../shing/restracker.hpp"
#include "../shing/moremath.hpp"
#include "../shing/asynca.hpp"
#include "../shing/consoline.hpp"
#include "../shing/consolearg.hpp"
#include "../servo/settingsspecifier.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace Gnaf{
class TestThingo{
   public:
   //-------------------------------------------------------
   Point2D latlong;
   string name;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class HomeTabes:public serVO::CrocMetaBase, public Shing::Tracked<HomeTabes>{
   public:
   //-------------------------------------------------------
   string teachpass;
   int number = 0;
   vector<TestThingo> thingos;
   bool has_input = false;
   string address_input;
   string address_output;
   bool has_any_match = false;
   string closest_match;
   bool has_gna_code = false;
   string gna_code;
   bool has_latlong = false;
   string latter_str;
   string longer_str;
   float latter = 0;
   float longer = 0;
   //-------------------------------------------------------
   HomeTabes();
   HomeTabes(vector<string>& rargs, map<string, string>& getvars, serVO::acSESH& leacsesh, serVO::Servo& leservo, const shringer& pather);
   void croc_init(serVO::Connection& conn);
   void croc_rpc(serVO::Connection& conn, Shing::RPC r);
   void croc_proc(serVO::CrocContext& cro);
   shringer metabase_name();
   void croform(serFORM& form);
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class Gnaffer{
   public:
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
DECLschdangler(gnaffer, Gnaffer);
}; // end namespace Gnaf
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

