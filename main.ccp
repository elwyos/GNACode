//////////////////////////////////////////////////////////////////////////////////////////
#bodinc gnaf csvlooper shing.strext addieparser consolearg
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
++/test_addies()
   int max_index = gnaf->addies.maxPossibleIndex();
   
   srand(time(NULL));
   \looprange i[0..200)
      
      int leindex = intrandbetween(0, max_index);
      auto recca = gnaf->addies.get(leindex);
      \if recca
         recca->print();
      \else
         \\/[196] "Missing index : " [220] leindex
      
//////////////////////////////////////////////////////////////////////////////////////////
++/test_act_csvs(const string& filter)
   
   schfile datadir("/home/yose/dev/geo/gnafpsvs");
   
   vector<string> kids = datadir.stringkids();
   \sloop kids
      \if Morestring::beginsWith(moo, "ACT_")
         
         string actual_name = Morestring::cutBegin(moo, "ACT_");
         \if Morestring::endsWith(actual_name, ".psv")
            actual_name = Morestring::cutEnd(actual_name, ".psv");
            
         \if Morestring::contains(__toLower(actual_name), filter)
            oustoust[33] << actual_name << ozendl;
            Shing::CSVLooper looper(datadir / moo, true, '|');
            looper.printAll(10);
            oustoust[196].ruler();
//////////////////////////////////////////////////////////////////////////////////////////
++/main_func(CargParser& parsa) -> bool
   
   using namespace Gnaf;
   
   \if parsa("test", "test CSV")
      test_act_csvs(__chompends(parsa--.parseUntilEnd()));
      return 0;
   \elif parsa("addy", "test addresses from testaddresses.strext")
      
      Strext lestrext;
      lestrext.loadFile("bundle/testaddresses.strext");
      \sloop lestrext
         AddieParser adipa(moo.tag);
         \oustboxer 240
         \\/ [240] "Raw Input : " [250] moo.tag
         \\/ [240] "Cleansed into: " \
         adipa.ret.print();
         
         oustoust[240].ruler();
         auto vec = gnaf->findMatches(adipa.ret);
         \sloop vec, moo, counta
            \if counta >= 7
               \\/ "and a " (vec.size() - counta) " more.."
               break;
         
            oustoust[33] << moo.score >> 250 << " : ";
            moo.address->print();
         
         
         
         
   \elif parsa("custfill", "fill specifying folder and state")
      
      string foldername = parsa--.parseFileName();
      string statename = __toUpper(parsa--.parseLabel());
      
      \\/ [33] "Foldername : " [250] foldername [250]" | " [220]statename
      gnaf->gnaf_psv_folder = foldername;
      gnaf->gnaf_state_code = statename;
      \\/ [220] "starting..."
      gnaf->fill();
      return 0;
      
      
   \elif parsa("fill", "fill address data (VIC only for now)")
      gnaf->fill();
      return 0;
   \elif parsa("randtest", "test random addresses")
      test_addies();
   \elif parsa("buildtest", "test buildings")
      gnaf->testBuildingMap();
   \elif parsa("naives", "naive search")
      gnaf->naiveStreetSearch(parsa--.parseUntilEnd());
   \elif parsa("servo")
      return true; // allow servo
   \else
      parsa.unhandled();
   
   
   return false;
