//  --  Script Scenario Name  --
static char const *scenario = "Rerouting";

#include "includes/general_setup.cc"
#include "includes/tracing.cc"
#include "network_templates/8.cc"
#include "network_templates/functions.cc"



/*

Inside the Point-to-Point Helper file at:
  "ns-3.36.1/src/point-to-point/helper/point-to-point-helper.h"
at the "public" part (line 46 - 197) insert the line:
  "auto InstallWithoutContainer(Ptr<Node> a, Ptr<Node> b);"



WILL ONLY WORK IF you

1. open src/point-to-point/model/point-to-point-channel.cc and create the following method:

void
PointToPointChannel::SetDelay (Time const &time)
{
  m_delay = time;
}

2. afterwards go to src/point-to-point/model/point-to-point-channel.h and add
the following line to the public methods

void SetDelay (Time const &time);

*/




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


  // end_time = start_time + fullPass;
  // simulationEndTime = Seconds (end_time);     // Set simulation time
  // sinkApp.Stop ( simulationEndTime );
  // sourceApp.Stop ( simulationEndTime );



  // param: netdevice, netdevice, channel, oppo_data = true, shifted_start
  // ScheduleDataRateAndDelay(d0_link_0_2, d1_link_0_2, link_0_2, true, 0);
  // ScheduleDataRateAndDelay(d0_link_0_2, d1_link_0_2, link_channel[0], true, 0);
  // ScheduleDataRateAndDelay(link_devices[0][0],link_devices[0][1], link_channel[0], true, 0);
 
 
  ScheduleDataRateAndDelay(0);
  ScheduleDataRateAndDelay(1);
  ScheduleDataRateAndDelay(2);
  ScheduleDataRateAndDelay(3);
  ScheduleDataRateAndDelay(4);
  ScheduleDataRateAndDelay(5);
  ScheduleDataRateAndDelay(6);
  ScheduleDataRateAndDelay(7);



  // ScheduleDataRateAndDelay(1, 5);
  // ScheduleDataRateAndDelay(2, 0, false);
  // ScheduleDataRateAndDelay(d2, d3, channel2, true, 0);

  // Simulator::Schedule (Seconds (1), &ChangeDataRate, channel, 0.342);
  


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
  (link_container)->~NetDeviceContainer();
  new (link_container) NetDeviceContainer();
  (link_interface)->~Ipv4InterfaceContainer();
  new (link_interface) Ipv4InterfaceContainer();
  resetTracingVars();   // Prepare for next TCP version simulation


  // Ptr<PointToPointNetDevice> link_devices[8][2];
  // Ptr<PointToPointChannel> link_channel[8];
  // NetDeviceContainer link_container[8];
  // Ipv4InterfaceContainer link_interface[8];

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
  // run("NewReno");
  run("Vegas");

}

