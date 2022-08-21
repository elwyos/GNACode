//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#include servo.servo shing.asynca consoline consolearg
#bodinc main gnaf addieparser
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//#bodinc redis.shingsplice
//////////////////////////////////////////////////////////////////////////////////////////
// dasq.libs = cryptopp rt dl z ssl crypto curl
// dasq.packages = libusockets servo telepatia
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
HomeTabes/croform(serFORM& form)
   
   \.h1 "GNA Code Generator!"
   
   <hr>.s3
   
   <p> "Enter an address here (VIC ONLY FOR NOW)"
      >.offk "(note: ambiguous addresses like corner addresses won't work..)"
   
   \.fieldtitle "The (vic only) Address"
   \&field address_input
      -placeholder 124 La Trobe St, Melbourne VIC 3000
      \\width: 100%
      `multiline
   
      
   \.butt "Get my GNA Code!"
      /click
         \\/ "Gots address to process : " address_input
         
         \syn \\has_input\\ = true;
         
         AddieParser adipa(address_input);
         
         address_output = adipa.ret.toStandardisedFormat();
            
            
         auto vec = gnaf->findMatches(adipa.ret);
         
         \if vec.size() != 0
         
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
            
         \else
            has_any_match = false;
            has_gna_code = false;
            has_latlong = false;
            
         // we're gonna have to clean that up someday
         
         
         

         
         
         // so we should have an address thing
         
   
   \.-------------------------
   
   <> \@has_input
      
      \if !has_input
         >.offk "Input an address first.."
      \else
         \.fieldtitle "standardised output (not checked with GNAF database):"
         \.h3 address_output
         
         \if has_any_match
            \.fieldtitle "closest database match (may be far off, we're doing naive street checking):"
            \.h3 closest_match
               \\color: #941
         \else
            >.offk "no match on the database (it's a work in progress..)"
         
         
         \if has_gna_code
            \.fieldtitle "GNA Code"
            \.h3 gna_code

            >.offk.sm "latitude : " + latter_str
            >.offk.sm ", longitude : " + longer_str
         
         <>.sm "also featuring the fastest geolocation service (VIC) in the world.."
      
   \.-------------------------
   
   \.leaflet \@has_input
      `latlong has_latlong ? latter : -33.8397, has_latlong ? longer : 151.0882
      
      \if has_latlong
         \.leafmark
            `latlong latter, longer
            \.leaftooltip
               <> gna_code
      
   
   \.-------------------------
      
   
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
\servo.generic.main

   bool allow_servo = Gnaf::main_func(parsa);
   \if !allow_servo
      return 0;
      
   \\/ "Syncing street name metas..."
   Gnaf::gnaf->syncStreetNameMetas();
   \\/ "Ready!"

//////////////////////////////////////////////////////////////////////////////////////////
\servo.settings
   devport: 3322
   devsslport: 4422
   title: "GNA Codes"
   desc: "GNA Code Making!!"
   lottie: 1
   shangterm: 1
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
\css 
    :root {
        --background-body: #000;
     };
