////////////////////////////////////////////////////////////
#include "gnaf.cl.hpp"
#include "../shing/csvlooper.hpp"
#include "../shing/strext.hpp"
#include "addieparser.cl.hpp"
#include "../shing/consolearg.hpp"
#include "main.cl.hpp"
#include "main.ceeferinc.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace Gnaf{
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void test_addies(){
   int max_index = gnaf->addies.maxPossibleIndex();
   srand(time(NULL));
   { int _rangetotal = 200;
   for(int i = 0; i < _rangetotal; i++){
      int leindex = intrandbetween(0, max_index);
      auto recca = gnaf->addies.get(leindex);
      if(recca){
         recca->print();
      }
      else {
         croust>>(196)<<"Missing index : ">>(220)<<(leindex) << ozendl;
      }
   }}
}
void test_act_csvs(const string& filter){
   schfile datadir("/home/yose/dev/geo/gnafpsvs");
   vector<string> kids = datadir.stringkids();
   sloopvec(kids, moo)
      if(Morestring::beginsWith(moo, "ACT_")){
         string actual_name = Morestring::cutBegin(moo, "ACT_");
         if(Morestring::endsWith(actual_name, ".psv")){
            actual_name = Morestring::cutEnd(actual_name, ".psv");
         }
         if(Morestring::contains(__toLower(actual_name), filter)){
            oustoust[33] << actual_name << ozendl;
            Shing::CSVLooper looper(datadir / moo, true, '|');
            looper.printAll(10);
            oustoust[196].ruler();
         }
      }
   sloopend
}
bool main_func(CargParser& parsa){
   using namespace Gnaf;
   if(parsa("test", "test CSV")){
      test_act_csvs(__chompends(parsa--.parseUntilEnd()));
      return 0;
   }
   else if(parsa("addy", "test addresses from testaddresses.strext")){
      Strext lestrext;
      lestrext.loadFile("bundle/testaddresses.strext");
      sloopvec(lestrext, moo)
         AddieParser adipa(moo.tag);
         OustBoxer oustboxer53(240);
         croust>>(240)<<"Raw Input : ">>(250)<<(moo.tag) << ozendl;
         croust>>(240)<<"Cleansed into: ";
         adipa.ret.print();
         oustoust[240].ruler();
         auto vec = gnaf->findMatches(adipa.ret);
         sloopveccounta(vec, moo, counta)
            if(counta >= 7){
               croust<<"and a "<<(vec.size() - counta)<<" more.." << ozendl;
               break;
            }
            oustoust[33] << moo.score >> 250 << " : ";
            moo.address->print();
         sloopend
      sloopend
   }
   else if(parsa("custfill", "fill specifying folder and state")){
      string foldername = parsa--.parseFileName();
      string statename = __toUpper(parsa--.parseLabel());
      croust>>(33)<<"Foldername : ">>(250)<<(foldername )>>(250)<<" | ">>(220)<<(statename) << ozendl;
      gnaf->gnaf_psv_folder = foldername;
      gnaf->gnaf_state_code = statename;
      croust>>(220)<<"starting..." << ozendl;
      gnaf->fill();
      return 0;
   }
   else if(parsa("fill", "fill address data (VIC only for now)")){
      gnaf->fill();
      return 0;
   }
   else if(parsa("randtest", "test random addresses")){
      test_addies();
   }
   else if(parsa("buildtest", "test buildings")){
      gnaf->testBuildingMap();
   }
   else if(parsa("naives", "naive search")){
      gnaf->naiveStreetSearch(parsa--.parseUntilEnd());
   }
   else if(parsa("servo")){
      return true; // allow servo
   }
   else {
      parsa.unhandled();
   }
   return false;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
}; // end namespace Gnaf
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

