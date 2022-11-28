/*      --  Congestion Script -- 
//  --------------------------------
    Network nodes: 21.142.213 satellites (COME ON ELON!, WHAT 'U GOT?)
    Network template: SKRIV DET!

    ------------------------- 
// - Default Network Topology - \\

       Wifi 10.1.3.0
                     AP
      *    *    *    *
      |    |    |    |    10.1.1.0
     n5   n6   n7   n0 -------------- n1   n2   n3   n4
                       point-to-point  |    |    |    |
                                       ================
                                         LAN 10.1.2.0

*/



// REPLACE WITH A REAL SCRIPT!!!
//
// KEEP AND UPDATE THE TOP COMMENT/EXPLANATION


///////////////////////////////////////////
/////  --  Placeholder Code  --  
//  NS-3 need "functioning code" for parser-files to be recognized
///////////

#include "ns3/core-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ScratchSimulator");

int 
main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("Scratch Simulator");

  Simulator::Run ();
  Simulator::Destroy ();
}
