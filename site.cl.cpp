////////////////////////////////////////////////////////////
#include "main.cl.hpp"
#include "gnaf.cl.hpp"
#include "addieparser.cl.hpp"
#include "site.cl.hpp"
#include "site.ceeferinc.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace Gnaf{
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
void HomeTabes::croform(serFORM& form){
   { form.cOPENER(":h1"); form.cARGER("n", "GNA Code Generator!");form.cCLOSER();
   }
   { form.cNODE(":hr")
      .enCLASS("s3")
      ;}
   { serVO::FormPopper _poppa(&form); form.cOPENER(":p"); form.cARGER("n", "Enter an address here (VIC ONLY FOR NOW)");form.cKIDSTART("i");
      { form.cOPENER(":span"); form.cARGER("n", "(note: ambiguous addresses like corner addresses won't work..)");form.cCLOSER()
         .enCLASS("offk")
         ;}
   _poppa.pop();
   }
   { form.cOPENER(":div"); form.cARGER("n", "The (vic only) Address");form.cCLOSER()
      .enCLASS("fieldtitle")
      ;}
   form.iso(address_input,[this](auto& form){;form.refFIELD(address_input)
   .enMULTILINE()
      .cloATTRIBBUILDUP("placeholder", "124 La Trobe St, Melbourne VIC 3000")
      .cloATTRIBBFLUSH()
      .cloSTYLEBUILDUP("width:""100%")
      .cloSTYLEBFLUSH()
      ;}, "address_input @ nd:+gnaf/site.ccp:28");
   { form.cOPENER("btn"); form.cARGER("n", "Get my GNA Code!");form.cCLOSER()
      .cloHANDLER(true,0, [this](auto& evt){
         croust<<"Gots address to process : "<<(address_input) << ozendl;
         
         has_input = true;;
         syn(has_input);
         AddieParser adipa(address_input);
         address_output = adipa.ret.toStandardisedFormat();
         auto vec = gnaf->findMatches(adipa.ret);
         if(vec.size() != 0){
            auto fronta = vec.front();
            closest_match = fronta.address->toSimpleString();
            gna_code = fronta.address->plusCode();
            has_any_match = true;
            has_gna_code = true;
            has_latlong = true;
            latter_str = fronta.address->geocode_lat;
            longer_str = fronta.address->geocode_lng;
            latter = Morestring::toFloat(fronta.address->geocode_lat);
            longer = Morestring::toFloat(fronta.address->geocode_lng);
         }
         else {
            has_any_match = false;
            has_gna_code = false;
            has_latlong = false;
         }
      })
      ;}
   form.ruler();
   
   form.iso(has_input,[this](auto& form){;{ serVO::FormPopper _poppa(&form); form.cOPENER(":div");form.cKIDSTART("i");
      if(!has_input){
         { form.cOPENER(":span"); form.cARGER("n", "Input an address first..");form.cCLOSER()
            .enCLASS("offk")
            ;}
      }
      else {
         { form.cOPENER(":div"); form.cARGER("n", "standardised output (not checked with GNAF database):");form.cCLOSER()
            .enCLASS("fieldtitle")
            ;}
         { form.cOPENER(":h3"); form.cARGER("n", address_output);form.cCLOSER();
         }
         if(has_any_match){
            { form.cOPENER(":div"); form.cARGER("n", "closest database match (may be far off, we're doing naive street checking):");form.cCLOSER()
               .enCLASS("fieldtitle")
               ;}
            { form.cOPENER(":h3"); form.cARGER("n", closest_match);form.cCLOSER()
               .cloSTYLEBUILDUP("color:""#941")
               .cloSTYLEBFLUSH()
               ;}
         }
         else {
            { form.cOPENER(":span"); form.cARGER("n", "no match on the database (it's a work in progress..)");form.cCLOSER()
               .enCLASS("offk")
               ;}
         }
         if(has_gna_code){
            { form.cOPENER(":div"); form.cARGER("n", "GNA Code");form.cCLOSER()
               .enCLASS("fieldtitle")
               ;}
            { form.cOPENER(":h3"); form.cARGER("n", gna_code);form.cCLOSER();
            }
            { form.cOPENER(":span"); form.cARGER("n", "latitude : " + latter_str);form.cCLOSER()
               .enCLASS("offk"" ""sm")
               ;}
            { form.cOPENER(":span"); form.cARGER("n", ", longitude : " + longer_str);form.cCLOSER()
               .enCLASS("offk"" ""sm")
               ;}
         }
         { form.cOPENER(":div"); form.cARGER("n", "also featuring the fastest geolocation service (VIC) in the world..");form.cCLOSER()
            .enCLASS("sm")
            ;}
      }
   _poppa.pop();
   }}, "has_input @ nd:+gnaf/site.ccp:79");
   form.ruler();
   
   form.iso(has_input,[this](auto& form){;{ serVO::FormPopper _poppa(&form); form.cOPENER("lf");form.cKIDSTART("i");
      if(has_latlong){
         { serVO::FormPopper _poppa(&form); form.cOPENER("lm");form.cKIDSTART("i");
            { serVO::FormPopper _poppa(&form); form.cOPENER("lt");form.cKIDSTART("i");
               { form.cOPENER(":div"); form.cARGER("n", gna_code);form.cCLOSER();
               }
            _poppa.pop();
            }
         _poppa.pop()
         .enLATLONG(latter,longer)
            ;}
      }
   _poppa.pop()
   .enLATLONG(has_latlong ? latter : -33.8397,has_latlong ? longer : 151.0882)
      ;}}, "has_input @ nd:+gnaf/site.ccp:106");
   form.ruler();
   
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
HomeTabes::HomeTabes() : CrocMetaBase(serVO::acSESH(), NULL, "nuffer")
{}
HomeTabes::HomeTabes(vector<string>& rargs, map<string, string>& getvars, serVO::acSESH& leacsesh, serVO::Servo& leservo, const shringer& pather) : CrocMetaBase(leacsesh, &leservo, "HomeTabes")
{metainit(leservo); metasetup(leacsesh, rargs, getvars); }
void HomeTabes::croc_init(serVO::Connection& conn){
linkConn(conn);
}
shringer HomeTabes::metabase_name(){
return shringer("HomeTabes");
}
void HomeTabes::croc_rpc(serVO::Connection& conn, Shing::RPC r){
      croust>>(33)<<"What Gots : ">>(250)<<(r->method() )<<" : "<<(r->strarg(0)) << ozendl;
}
void HomeTabes::croc_proc(serVO::CrocContext& cro){
linkConn(cro.conn);
{ serVO::CrocForm form(this, cro.stringer, "form");
croform(form);
}
}
serVO::RouteSpecifier _routespec_HomeTabes("/", "bundle/stdweb.dir/stdweb.hpl", serVO::landing_func_maker<HomeTabes>(), serVO::reroute_func_maker<HomeTabes>());
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
////////////////////////////////////////////////////////////
}; // end namespace Gnaf
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
int main(int argc, char * argv[]){ CargParser parsa(argc, argv); 
   bool allow_servo = Gnaf::main_func(parsa);
   if(!allow_servo){
      return 0;
   }
   croust<<"Syncing street name metas..." << ozendl;
   Gnaf::gnaf->syncStreetNameMetas();
   croust<<"Ready!" << ozendl;
; return serVO::servomain(argc, argv); }
serVO::SettingsSpecifier _srvstspc0("devport", 3322);
serVO::SettingsSpecifier _srvstspc1("devsslport", 4422);
serVO::SettingsSpecifier _srvstspc2("title", "GNA Codes");
serVO::SettingsSpecifier _srvstspc3("desc", "GNA Code Making!!");
serVO::SettingsSpecifier _srvstspc4("lottie", 1);
serVO::SettingsSpecifier _srvstspc5("shangterm", 1);
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

