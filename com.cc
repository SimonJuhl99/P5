//  --  Script Scenario Name  --
static char const *scenario = "Rerouting";

#include "includes/general_setup.cc"
#include "includes/tracing.cc"
#include "network_templates/8.cc"
#include "network_templates/functions.cc"


// static std::map<string,bool> config, default_config;

// static std::map<const char*,bool> default_config;
// std::map<const char*,bool> default_config["link_error"] = false; 
// static std::map<string,bool> default_config["link_error"] = false; 
// static std::map<string,bool> default_config["moving"] = false; 
// static std::map<string,bool> default_config["rerouting"] = false; 
// static std::map<string,bool> default_config["congestion"] = false;

// default_config["link_error"] = false; 
// default_config["moving"] = false; 
// default_config["rerouting"] = false; 
// default_config["congestion"] = false;


// void printIt(std::map<int,int> m) {
// void printIt(std::map<string,int> m) {
void printIt(std::map<string,bool> m) {
    // for(std::map<int,int>::iterator it=m.begin();it!=m.end();++it)
    // for(std::map<string,int>::iterator it=m.begin();it!=m.end();++it)
    cout << "Configuration is: " << std::endl; 
    cout << "simulation time: " << end_time << std::endl;
    for(std::map<string,bool>::iterator it=m.begin();it!=m.end();++it)
        cout << it->first<<": "<<it->second<< std::endl;
    cout << "\n";
}


int
// run (string tcp_version, bool link_error = false, std::map<string,bool> config = default_config)
run (string tcp_version, std::map<string,bool> config = default_config)
// run (string tcp_version, bool link_error = false)
{
  /* --------------------------------------------------------
  //  --    General Setup   --
  //  --  DON'T TOUCH THIS!  --
  */

  config.insert(default_config.begin(), default_config.end());    // config takes presidence here.


  if (config["moving"] == true) {
    if (config["rerouting"] == true) {
      end_time = start_time + 1174;
      // end_time = start_time + 1162;
      // end_time = start_time + 1242;
      simulationEndTime = Time(Seconds (end_time));     // Set simulation time
      defaultDatarate = DataRate("5Mbps");
      delay = 0.005;
    }
    else {
      end_time = start_time + 772;
      simulationEndTime = Time(Seconds (end_time));     // Set simulation time
    }
  }



  printIt(config);

  string error_str = "";
  string error_activated_str = "";
  // if ( link_error ) {   // If packetdrop on rerouting is enabled
  if ( (config["link_error"] == true)  && (config["rerouting"] == true) ) {   // If packetdrop on rerouting is enabled
    error_str = "-error";
  }

  AnimationInterface anim = build_network(tcp_version, error_str);   // Network Setup
  auto [source, sinkAddress] = setupDefaultNodeTraffic(tcp_version, error_str, config);   // Creating Default Sink & Source



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


  if ( config["moving"] == true) {   // If moving satellites is enabled

    if (config["rerouting"] == true) {

      int shift_first = 30;
      int shift_second = 340;
      // int shift_second = 420;
      // int shift_second = 500;

      ScheduleDataRateAndDelay(4, shift_first);
      ScheduleDataRateAndDelay(6, shift_first);

      ScheduleDataRateAndDelay(3, shift_second);
      ScheduleDataRateAndDelay(5, shift_second);
    }
    else {
      ScheduleDataRateAndDelay(0);
      ScheduleDataRateAndDelay(1);
      ScheduleDataRateAndDelay(2);
      ScheduleDataRateAndDelay(3);
      ScheduleDataRateAndDelay(4);
      ScheduleDataRateAndDelay(5);
      ScheduleDataRateAndDelay(6);
      ScheduleDataRateAndDelay(7);
    }



  }


  if ( config["rerouting"] == true) {   // If moving satellites is enabled
    float first_reroute_time = start_time + 100.07;
    float second_reroute_time = start_time + 150.07;

    if (config["moving"] == true) {
      first_reroute_time = start_time + 712.007;
      second_reroute_time = start_time + 1500.007;
    }


    // Get node 6 and its ipv4, to prepare changing route
    Ptr<Node> n3 = node.Get (6);   // Grap third node (before forking)
    Ptr<Ipv4> n3ipv4 = n3->GetObject<Ipv4> ();
    // The first interfaceIndex is 0 for loopback, then the first p2p connection is numbered 1, numbered by order of creation.
    uint32_t n3_to_n4_connection = 1;    // Connection index between node 3 & 4
    uint32_t n3_to_n5_connection = 2;    // Connection index between node 3 & 5



    //  ---------------------------------------
    //  --  Simulation Rerouting Scheduling  --

    // SetDown & SetUp opens and closes that specific connection.
    Simulator::Schedule (Seconds (start_time + 0.00001), &Ipv4::SetDown, n3ipv4, n3_to_n4_connection);
    
    Simulator::Schedule (Seconds (first_reroute_time), &Ipv4::SetUp, n3ipv4, n3_to_n4_connection);
    Simulator::Schedule (Seconds (first_reroute_time), &Ipv4::SetDown, n3ipv4, n3_to_n5_connection);

    if ( config["link_error"] == true ) {   // If packetdrop on rerouting is enabled
      linkDrops(6, first_reroute_time, true);
    }

    // Rorouting again
    Simulator::Schedule (Seconds (second_reroute_time), &Ipv4::SetUp, n3ipv4, n3_to_n5_connection);
    Simulator::Schedule (Seconds (second_reroute_time), &Ipv4::SetDown, n3ipv4, n3_to_n4_connection);

    if ( config["link_error"] == true ) {   // If packetdrop on rerouting is enabled
      linkDrops(5, second_reroute_time, true);
      linkDrops(6, second_reroute_time, false);
    }



    //  --  For usage in "prints"  --
    if ( config["link_error"] == true ) {   // If packetdrop on rerouting is enabled
      error_activated_str = "Rerouting has dropped packets on route change.";
    }
    else {
      error_activated_str = "Rerouting has dropped packets on route change.";
    }

  }


  if ( config["congestion"] == true) {   // If moving satellites is enabled
    int new_sink_node = 1;   // Link, not node... see network template scematic
    int new_sink_link = 1;   // Link, not node... see network template scematic
    int new_source = 5;   // Link, not node... see network template scematic

    Address sink2 = createSink(new_sink_node, new_sink_link, tcp_version, 0);
    BulkSendHelper source2 = createSource(new_source, tcp_version, sink2, error_str);

    sourceApp.Start ( Seconds ( source_start_time ) );
    sourceApp.Stop ( simulationEndTime );

  }


  



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

  // auto itr = stats.begin ();  
  
  Simulator::Destroy ();
  NS_LOG_INFO ("Simulation Done.");


  /////////////////////////////////////////////////
  //  --  Prepare variables for another run  --
  (&node)->~NodeContainer();
  new (&node) NodeContainer();

  // Ptr<PointToPointNetDevice> link_devices[8][2];
  // Ptr<PointToPointChannel> link_channel[8];
  // NetDeviceContainer link_container[8];
  // Ipv4InterfaceContainer link_interface[8];


  resetTracingVars();   // Prepare for next TCP version simulation

  return 1;
}


int
main (int argc, char *argv[])
{

  default_config["moving"] = false; 
  default_config["rerouting"] = false; 
  default_config["link_error"] = false; 
  default_config["congestion"] = false;

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

  std::map<string,bool> conf { 
                          // {"link_error", true},
                          // {"rerouting", true},
                          {"congestion", true}, 
                          {"moving", true}
                        };


  run("Bbr", conf);
  // run("NewReno", conf);
  // run("Vegas", conf);

}
