static char const *scenario = "Congestion";
#include "includes/general_setup.cc"
// #include "includes/tracing.cc"

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
/*
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
*/


//NS_LOG_COMPONENT_DEFINE ("TcpBulkSendExample");

static std::map<uint32_t, bool> firstCwnd;
static std::map<uint32_t, bool> firstRtt;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> cWndStream;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rttStream;
static std::map<uint32_t, uint32_t> cWndValue;

static uint32_t
GetNodeIdFromContext (std::string context)
{
  std::size_t const n1 = context.find_first_of ("/", 1);
  std::size_t const n2 = context.find_first_of ("/", n1 + 1);
  return std::stoul (context.substr (n1 + 1, n2 - n1 - 1));
}

static void
CwndTracer (std::string context,  uint32_t oldval, uint32_t newval)
{
  //NS_LOG_INFO("Cwnd Started");
  uint32_t nodeId = GetNodeIdFromContext (context);

  if (firstCwnd[nodeId])
    {
      *cWndStream[nodeId]->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd[nodeId] = false;
    }
  *cWndStream[nodeId]->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue[nodeId] = newval;
}

static void
TraceCwnd (std::string cwnd_tr_file_name, uint32_t nodeId)
{
  //NS_LOG_INFO("TraceCwnd Start");
  AsciiTraceHelper ascii;
  cWndStream[nodeId] = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::Connect ("/NodeList/" + std::to_string (nodeId) + "/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
                   MakeCallback (&CwndTracer));
}

static void
RttTracer (std::string context, Time oldval, Time newval)
{
  uint32_t nodeId = GetNodeIdFromContext (context);

  if (firstRtt[nodeId])
    {
      *rttStream[nodeId]->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt[nodeId] = false;
    }
  *rttStream[nodeId]->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
TraceRtt (std::string rtt_tr_file_name, uint32_t nodeId)
{
  AsciiTraceHelper ascii;
  rttStream[nodeId] = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::Connect ("/NodeList/" + std::to_string (nodeId) + "/$ns3::TcpL4Protocol/SocketList/0/RTT",
                   MakeCallback (&RttTracer));
}


int
run (string tcp_version)
{

  //bool tracing = false;
  uint32_t maxBytes = 0;
  std::string prefix_file_name = tcp_version;


//
// Explicitly create the nodes required by the topology (shown above).
//
  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (6);

  NodeContainer n0n2 = NodeContainer(nodes.Get(0), nodes.Get(2));
  NodeContainer n1n2 = NodeContainer(nodes.Get(1), nodes.Get(2));
  NodeContainer n2n3 = NodeContainer(nodes.Get(2), nodes.Get(3));
  NodeContainer n3n4 = NodeContainer(nodes.Get(3), nodes.Get(4));
  NodeContainer n3n5 = NodeContainer(nodes.Get(3), nodes.Get(5));

  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes);

  Ptr<ConstantPositionMobilityModel> mn0 = nodes.Get(0)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn1 = nodes.Get(1)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn2 = nodes.Get(2)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn3 = nodes.Get(3)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn4 = nodes.Get(4)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn5 = nodes.Get(5)->GetObject<ConstantPositionMobilityModel> ();
  

  mn0->SetPosition (Vector ( 0.5, 1.0, 0  ));
  mn1->SetPosition (Vector ( 1.0, 0.5, 0  ));
  mn2->SetPosition (Vector ( 1.0, 1.0, 0  ));
  mn3->SetPosition (Vector ( 1.5, 1.0, 0  ));
  mn4->SetPosition (Vector ( 1.5, 1.5, 0  ));
  mn5->SetPosition (Vector ( 2.0, 1.0, 0  ));

 

  NS_LOG_INFO ("Create channels.");

//
// Explicitly create the point-to-point link required by the topology (shown above).
//
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("5ms"));

  NetDeviceContainer d0d2 = pointToPoint.Install (n0n2);
  NetDeviceContainer d1d2 = pointToPoint.Install (n1n2);
  //pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Kbps"));
  NetDeviceContainer d2d3 = pointToPoint.Install (n2n3);
  //pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
  NetDeviceContainer d3d4 = pointToPoint.Install (n3n4);
  NetDeviceContainer d3d5 = pointToPoint.Install (n3n5);
//
// Install the internet stack on the nodes
//
  InternetStackHelper internet;
  internet.Install (nodes);

//
// We've got the "hardware" in place.  Now we need to add IP addresses.
//
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i2 = ipv4.Assign (d0d2);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign(d1d2);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  ipv4.Assign(d2d3);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i4 = ipv4.Assign(d3d4);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i5 = ipv4.Assign(d3d5);

  NS_LOG_INFO ("Create Applications.");

// 
// Add Animation 
//

   AnimationInterface anim (tcp_version + ".xml");

  anim.UpdateNodeSize(nodes.Get(0)->GetId(), .1, .1);
  anim.UpdateNodeSize(nodes.Get(1)->GetId(), .1, .1);
  anim.UpdateNodeSize(nodes.Get(2)->GetId(), .1, .1);
  anim.UpdateNodeSize(nodes.Get(3)->GetId(), .1, .1);
  anim.UpdateNodeSize(nodes.Get(4)->GetId(), .1, .1);
  anim.UpdateNodeSize(nodes.Get(5)->GetId(), .1, .1);

//
// Populate the routing nodes on each of the nodes. 
//
Ipv4GlobalRoutingHelper::PopulateRoutingTables();

//
// Create a BulkSendPtr<ConstantPositionMobilityModel> mn1 = nodes.Get(1)->GetObject<ConstantPositionMobilityModel> ();
  //Application and install it on node 0
//
  uint16_t port = 9;  // well-known echo port number


  BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (i3i4.GetAddress (1), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  BulkSendHelper source2 ("ns3::TcpSocketFactory", InetSocketAddress(i3i5.GetAddress(1), port));
  source2.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApps = source.Install (nodes.Get (0));
  sourceApps.Add(source2.Install(nodes.Get(1)));
  sourceApps.Start (Seconds (0.0));
  sourceApps.Stop (Seconds (190.0));

//
// Create a PacketSinkApplication and install it on node 1
//
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (nodes.Get (4));

  PacketSinkHelper sink2 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  sinkApps.Add(sink2.Install(nodes.Get(5)));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (190.0));

//
// Set up tracing if enabled
//
  /*
  if (tracing)
    {
      AsciiTraceHelper ascii;
      pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("Congestion.tr"));
      pointToPoint.EnablePcapAll ("Congestion", false);
    }
*/
//
// Capture packets: create pcap files. 
  /*
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("Congestion.tr");
  pointToPoint.EnableAsciiAll(stream);
  pointToPoint.EnablePcapAll("Congestion");
*/
//
// Now, do the actual simulation.
//
 // Setup tracing for congestion window on node 0 and write results to file
 if (tcp_version == "Bbr") {

 }
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceCwnd,
                       prefix_file_name + "-cwnd.data", 0);
  // Setup tracing for RTT
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRtt,
                       prefix_file_name + "-rtt.data", 0);

  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceCwnd,
                       prefix_file_name + "-cwnd2.data", 1);
  // Setup tracing for RTT
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRtt,
                       prefix_file_name + "-rtt2.data", 1);


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (200.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  Ptr<PacketSink> Sink_A = DynamicCast<PacketSink> (sinkApps.Get (0));
  Ptr<PacketSink> Sink_B = DynamicCast<PacketSink> (sinkApps.Get (1));
  std::cout << "Total Bytes Received at sink 1: " << Sink_A->GetTotalRx () << std::endl;
  std::cout << "Total Bytes Received at Sink 2: " << Sink_B->GetTotalRx () << std::endl;

  return 1;
}


int
main (int argc, char *argv[])
{
  ///////////////////
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


  run("Bbr");
  run("Vegas");
  run("NewReno");
}


