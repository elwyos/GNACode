////////////////////////////////////////////////////////////
#include "../shing/oust.hpp"
#include "address.cl.hpp"
#include "address.ceeferinc.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
namespace Gnaf{
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
AddieCompo::AddieCompo(){
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
AddieCompo::AddieCompo(const string& firststr, const string& secondstr){
   filled = true;
   value = firststr;
   postfix = secondstr;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void AddieCompo::printFirstPart(){
   if(value.size()){
      oustoust.printSmartColored(value);
   }
   else {
      if(valuenum != -1){
         croust>>(246)<<(valuenum );
      }
      oustoust.printSmartColored(firstvalpostfix);
      if(connector){
         croust>>(196)<<(string(1, connector) );
      }
      if(secvaluenum != -1){
         croust>>(246)<<(secvaluenum );
      }
      oustoust.printSmartColored(secvalpostfix);
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
string AddieCompo::stringifyFirstPart(){
   if(value.size()){
      return value;
   }
   else {
      string ret;
      if(valuenum != -1){
         ret.append(__toString(valuenum));
      }
      ret += firstvalpostfix;
      if(connector){
         ret.append(1, connector);
      }
      if(secvaluenum != -1){
         ret += __toString(secvaluenum);
      }
      ret += secvalpostfix;
      return ret;
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
string Address::toStandardisedFormat(){
   string ret;
   if(floorlevel){
      ret.append(floorlevel.postfix);
      ret += " ";
      if(floorlevel.valuenum != -1){
         ret.append(floorlevel.stringifyFirstPart());
         ret += " ";
      }
   }
   if(unitnumber){
      ret += unitnumber.postfix + " ";
      ret.append(unitnumber.stringifyFirstPart());
      ret +=  " ";
   }
   if(lot){
      ret += "lot " + lot.stringifyFirstPart() + " ";
   }
   if(propnumber){
      ret += propnumber.stringifyFirstPart() + " ";
   }
   if(streetname){
      ret += streetname.value + " ";
      ret += streetname.postfixlong + " ";
   }
   if(locality){
      ret += locality.value + " ";
   }
   if(state){
      ret += __toUpper(state.value) + " ";
   }
   if(postcode){
      ret += __toUpper(postcode.value);
   }
   return ret;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Address::print(){
   if(floorlevel){
      croust>>(floorlevel.postfix)<<(floorlevel.postfix )<<" ";
      if(floorlevel.valuenum != -1){
         floorlevel.printFirstPart();
         croust<<" ";
      }
   }
   if(unitnumber){
      croust>>(unitnumber.postfix)<<(unitnumber.postfix )<<" ";
      unitnumber.printFirstPart();
      croust<<" ";
   }
   if(lot){
      croust>>(220)<<"lot ";
      lot.printFirstPart();
      croust<<" ";
   }
   if(propnumber){
      croust>>(220)<<"[";
      propnumber.printFirstPart();
      croust>>(220)<<"]";
      croust<<" ";
   }
   if(streetname){
      croust>>(250)<<(streetname.value )<<" ";
      croust>>(streetname.postfixlong)<<(streetname.postfixlong )<<" ";
   }
   if(locality){
      croust.dtag(locality.value);
      croust<<" ";
   }
   if(state){
      croust.dtag(__toUpper(state.value));
      croust<<" ";
   }
   if(postcode){
      croust.dtag(postcode.value);
      croust<<" ";
   }
   croust << ozendl;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
}; // end namespace Gnaf
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

