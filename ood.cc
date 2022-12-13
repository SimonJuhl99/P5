//  --  Script Scenario Name  --
static char const *scenario = "Rerouting";

#include "includes/general_setup.cc"
#include "includes/tracing.cc"
#include "network_templates/8.cc"


// ----------------------------
//  --  New network OOD script
int
run (string tcp_version)
{

  DataRate bottleneckBandwidth ("1Mbps");
  Time bottleneckDelay = MilliSeconds (5);

  AnimationInterface anim = build_network(tcp_version);

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (bottleneckBandwidth));
  p2p.SetChannelAttribute ("Delay", TimeValue (bottleneckDelay));
  device_container[6] = p2p.Install (n5n6);   // Gammel d2d3

  NS_LOG_INFO ("Creating Applications.");

  uint16_t sinkPort = 8080;

  Address sinkAddress ( InetSocketAddress ( node_interface[7].GetAddress(1), sinkPort ) );     // setup sink interface on node 7
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress ( Ipv4Address::GetAny (), sinkPort ) );
  ApplicationContainer sinkApp = packetSinkHelper.Install( c.Get (7) );   // Install sink application on node 7

  sinkApp.Start ( Seconds (0) );
  sinkApp.Stop ( simulationEndTime );

  BulkSendHelper source ("ns3::TcpSocketFactory", sinkAddress);
  source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApp = source.Install ( c.Get (2) );

  sourceApp.Start ( Seconds ( start_time ) );
  sourceApp.Stop ( simulationEndTime );

  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ( tcp_version + ".tr" );
  p2p.EnableAsciiAll ( stream );
  p2p.EnablePcapAll ( tcp_version );

  NS_LOG_INFO ( "Time of start " << Seconds( start_time+0.00001 ) );
  // Setup tracing for congestion window on node 0 and write results to file
  Simulator::Schedule ( Seconds ( start_time + 0.00001 ), &TraceCwnd,
                        tcp_version + "-cwnd.data", 2 );
  // Setup tracing for RTT
  Simulator::Schedule ( Seconds ( start_time + 0.00001 ), &TraceRtt,
                        tcp_version + "-rtt.data", 2 );

  // Setup FlowMonitor and tracing for throughput
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  Simulator::Schedule (Seconds ( start_time + 0.00001 ), &TraceThroughput,
                      tcp_version + "-throughput.data", monitor);

  // Get node 3 and its ipv4, to prepare changing route
  Ptr<Node> n3 = c.Get (3);
  Ptr<Ipv4> n3ipv4 = n3->GetObject<Ipv4> ();
  // The first interfaceIndex is 0 for loopback, then the first p2p is numbered 1
  uint32_t n3ipv4ifIndex2 = 2;    // Connection between node 2 & 3.. I think

  Simulator::Schedule (Seconds (start_time + 1.02), &Ipv4::SetDown, n3ipv4, n3ipv4ifIndex2);


  NS_LOG_INFO ("\nStarting Simulation Using TCP " + tcp_version + ".");
  Simulator::Stop(simulationEndTime);
  Simulator::Run ();
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
  // run("NewReno");
}