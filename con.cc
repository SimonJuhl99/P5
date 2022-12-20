static char const *scenario = "Congestion";
#include "includes/general_setup.cc"
#include "includes/tracing.cc"
#include "network_templates/8.cc"
#include "network_templates/functions.cc"


int
run (string tcp_version, bool link_error = false)
{
  /* --------------------------------------------------------
  //  --    General Setup   --
  //  --  DON'T TOUCH THIS!  --
  */
  string error_str = "";
  string error_activated_str = "";
  if (link_error) {   // If packetdrop on rerouting is enabled
    error_str = "-error";
  }

  AnimationInterface anim = build_network(tcp_version, error_str);   // Network Setup
  auto [source, sinkAddress] = setupDefaultNodeTraffic(tcp_version, error_str);   // Creating Default Sink & Source


  /* --------------------------------------------------------
  //  --  Write Specific Scenario Script Under This  --
  //
  //      !!--!!  START FROM HERE!  !!--!!
  //
  //        ||     ||     ||     ||
  //        ||     ||     ||     ||
  //       \  /   \  /   \  /   \  /
  //        \/     \/     \/     \/
  */




  /////////////////////////////////////
  //  --  Active script part  --
  NS_LOG_INFO ("Creating Applications.");



  int new_sink_node = 1;   // Link, not node... see network template scematic
  int new_sink_link = 1;   // Link, not node... see network template scematic
  int new_source = 5;   // Link, not node... see network template scematic

  Address sink2 = createSink(new_sink_node, new_sink_link, tcp_version, 0);
  BulkSendHelper source2 = createSource(new_source, tcp_version, sink2, error_str);

  sourceApp.Start ( Seconds ( source_start_time ) );
  sourceApp.Stop ( simulationEndTime );



  /*
  //        /\     /\     /\     /\
  //       /  \   /  \   /  \   /  \
  //        ||     ||     ||     ||
  //        ||     ||     ||     ||
  //
  //      !!--!!  STOP HERE!  !!--!!
  //
  //  --  Write Specific Scenario Script Above This  --
   --------------------------------------------------------*/
  
  
  NS_LOG_INFO ("\n________________________________________\nStarting Simulation Using TCP " + tcp_version + ".\n" + error_activated_str );
  Simulator::Stop(simulationEndTime);
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Simulation Done.");

  // Ptr<PacketSink> Sink_A = DynamicCast<PacketSink> (sinkApp.Get (0));
  // Ptr<PacketSink> Sink_B = DynamicCast<PacketSink> (sinkApp.Get (1));
  // std::cout << "Total Bytes Received at sink 1: " << Sink_A->GetTotalRx () << std::endl;
  // std::cout << "Total Bytes Received at Sink 2: " << Sink_B->GetTotalRx () << std::endl;

  /////////////////////////////////////////////////
  //  --  Prepare variables for another run  --
  (&node)->~NodeContainer();
  new (&node) NodeContainer();
  (link_container)->~NetDeviceContainer();
  new (link_container) NetDeviceContainer();
  (link_interface)->~Ipv4InterfaceContainer();
  new (link_interface) Ipv4InterfaceContainer();
  resetTracingVars();   // Prepare for next TCP version simulation

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

  // run("Bbr");
  run("NewReno");
  run("Vegas");

}


