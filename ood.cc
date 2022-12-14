//  --  Script Scenario Name  --
static char const *scenario = "Rerouting";

#include "includes/general_setup.cc"
#include "includes/tracing.cc"
#include "network_templates/8.cc"
#include "network_templates/functions.cc"



int
run (string tcp_version)
{
    /* --------------------------------------------------------
  //  --    General Setup   --
  //  --  DON'T TOUCH THIS!  --
  */
  AnimationInterface anim = build_network(tcp_version);   // Network Setup
  setupDefaultNodeTraffic(tcp_version);   // Creating Default Sink & Source



  /* --------------------------------------------------------
  //  --  Write Specific Scenario Script Under This  --
  //  --  START FROM HERE!  --
  */


  // Create bottleneck in one of the links

  DataRate bottleneckBandwidth ("1Mbps");
  Time bottleneckDelay = MilliSeconds (5);
  p2p.SetDeviceAttribute ("DataRate", DataRateValue (bottleneckBandwidth));
  p2p.SetChannelAttribute ("Delay", TimeValue (bottleneckDelay));
  device_container[6] = p2p.Install (n5n6);   // Gammel d2d3




  /////////////////////////////////////
  //  --  Active script part  --
  NS_LOG_INFO ("Creating Applications.");


  // Get node 3 and its ipv4, to prepare changing route
  Ptr<Node> n3 = c.Get (3);   // Grap third node (before forking)
  Ptr<Ipv4> n3ipv4 = n3->GetObject<Ipv4> ();
  // The first interfaceIndex is 0 for loopback, then the first p2p is numbered 1
  uint32_t n3_to_n4_index = 2;    // Connection index between node 3 & 4
  // uint32_t n3_to_n5_index = 3;    // Connection index between node 3 & 5




  //  ---------------------------------------
  //  --  Simulation Rerouting Scheduling  --

  float reroute_time = start_time + 6.01;

  // SetDown & SetUp opens and closes that specific connection.
  Simulator::Schedule (Seconds (start_time + 0.00001), &Ipv4::SetDown, n3ipv4, n3_to_n4_index);
  Simulator::Schedule (Seconds (reroute_time), &Ipv4::SetUp, n3ipv4, n3_to_n4_index);
  // Simulator::Schedule (Seconds (reroute_time), &Ipv4::SetDown, n3ipv4, n3_to_n5_index);
  Simulator::Schedule (Seconds (reroute_time), &ActivateError, device_container[4].Get (1), true);




  // Simulator::Schedule (Seconds (start_time + 6), &ActivateError, device_container[4].Get (1), false);
  // Simulator::Schedule (Seconds (start_time + 6), &Ipv4::SetUp, n3ipv4, n3_to_n5_index);
  // Simulator::Schedule (Seconds (start_time + 6), &Ipv4::SetDown, n3ipv4, n3_to_n3_index);
  // Simulator::Schedule (Seconds (start_time + 6), &ActivateError, device_container[3].Get (1), true);

  // Simulator::Schedule (Seconds (start_time + 9), &ActivateError, device_container[3].Get (1), false);
  // Simulator::Schedule (Seconds (start_time + 9), &Ipv4::SetUp, n3ipv4, n3_to_n4_index);
  // Simulator::Schedule (Seconds (start_time + 9), &Ipv4::SetDown, n3ipv4, n3_to_n5_index);
  // Simulator::Schedule (Seconds (start_time + 9), &ActivateError, device_container[4].Get (1), true);



  /*
  //  --  STOP HERE!  --
  //  --  Write Specific Scenario Script Above This  --
   --------------------------------------------------------*/


  NS_LOG_INFO ("\nStarting Simulation Using TCP " + tcp_version + ".");
  Simulator::Stop(simulationEndTime);
  Simulator::Run ();

  // monitor->CheckForLostPackets ();
  // Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  // FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

  // //std::ostringstream os;

  // for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  //   {
  //     Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

  //     std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
  //     std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
  //     std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
  //     std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / simulationEndTime.GetSeconds () / 1000 / 1000  << " Mbps\n";
  //     std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
  //     std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
  //     std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / simulationEndTime.GetSeconds () / 1000 / 1000  << " Mbps\n";
  //     //i->second.flowInterruptionsHistogram.SerializeToXmlStream(os, 2, "flowInterruptionsHistogram");
  //     //std::cout << "  Flow Interruptions Histogram:\n" << i->second.flowInterruptionsHistogram << "\n------------------\n";
  //   }

  Simulator::Destroy ();
  NS_LOG_INFO ("Simulation Done.");

  (&c)->~NodeContainer();
  new (&c) NodeContainer();

  return 1;
}


int
main (int argc, char *argv[])
{
  /////////////////////////
  // --  Argument Area
  bool verbose = true;
  uint32_t nCsma = 3;
  uint32_t nWifi = 3;
  bool tracing = false;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);


  //  ----------------------------------
  //  Actual Script Running Section

  run("Bbr");
  run("Vegas");
  run("NewReno");
}
