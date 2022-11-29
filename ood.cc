#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

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
using std::string;
using std::cout;

NS_LOG_COMPONENT_DEFINE ("two_routes");

uint32_t prev = 0;
Time prevTime = Seconds (0);

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

static void
TraceThroughput (std::string tp_tr_file_name, Ptr<FlowMonitor> monitor)
{
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  // pointer to first value in stats
  auto itr = stats.begin ();
  // current time
  Time curTime = Now ();
  // ofstream is an output stream class to operate on files
  // bitwise 'or' on either allowing output or appending to a stream
  std::ofstream thr (tp_tr_file_name, std::ios::out | std::ios::app);
  // write to stream:
  // current time and
  // transmitted bytes since last transmission divided by
  // the time since last transmission
  // which equals throughput (transmitted data over time)
  thr <<  curTime << " " << 8 * (itr->second.txBytes - prev) / (1000 * 1000 * (curTime.GetSeconds () - prevTime.GetSeconds ())) << std::endl;
  // current time and current transmitted bytes which for next iteration becomes 'previous'
  prevTime = curTime;
  prev = itr->second.txBytes;
  // recursive call every x seconds
  Simulator::Schedule (Seconds (0.02), &TraceThroughput, tp_tr_file_name, monitor);
}


int
run (std::string tcp_version)
{

  uint32_t maxBytes = 0;
  std::string transportProtocol = "ns3::Tcp" + tcp_version;
  std::string prefix_file_name =  tcp_version;

  Time simulationEndTime = Seconds (4.1);
  DataRate bottleneckBandwidth ("2Mbps");
  Time bottleneckDelay = MilliSeconds (5);
  DataRate regLinkBandwidth = DataRate (4 * bottleneckBandwidth.GetBitRate ());
  Time regLinkDelay = MilliSeconds (5);

  Config::SetDefault ("ns3::BulkSendApplication::SendSize", UintegerValue (100000));
  //Config::SetDefault ("ns3::BulkSendApplication::DataRate", StringValue ("448kb/s"));
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transportProtocol)));
  Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (10));

  LogComponentEnable("two_routes", LOG_LEVEL_ALL);


  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (6);

  NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
  NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));
  NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));
  NodeContainer n3n4 = NodeContainer (c.Get (3), c.Get (4));
  NodeContainer n1n5 = NodeContainer (c.Get (1), c.Get (5));
  NodeContainer n5n3 = NodeContainer (c.Get (5), c.Get (3));

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c);

  InternetStackHelper internet;
  internet.Install (c);

  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (regLinkBandwidth));
  p2p.SetChannelAttribute ("Delay", TimeValue (regLinkDelay));
  NetDeviceContainer d0d1 = p2p.Install (n0n1);

  NetDeviceContainer d1d2 = p2p.Install (n1n2);

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (bottleneckBandwidth));
  p2p.SetChannelAttribute ("Delay", TimeValue (bottleneckDelay));
  NetDeviceContainer d2d3 = p2p.Install (n2n3);

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (regLinkBandwidth));
  p2p.SetChannelAttribute ("Delay", TimeValue (regLinkDelay));
  NetDeviceContainer d3d4 = p2p.Install (n3n4);

  NetDeviceContainer d1d5 = p2p.Install (n1n5);

  NetDeviceContainer d5d3 = p2p.Install (n5n3);


  // We add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4.Assign (d0d1);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  ipv4.Assign (d1d2);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  ipv4.Assign (d2d3);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i4 = ipv4.Assign (d3d4);     // need container to get address more easily later on

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  ipv4.Assign (d1d5);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  ipv4.Assign (d5d3);


  // Create the animation object and configure for specified output
  AnimationInterface anim ( tcp_version + ".xml");      // NetAnim Output File   <---

  Ptr<ConstantPositionMobilityModel> mn0 = c.Get (0)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn1 = c.Get (1)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn2 = c.Get (2)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn3 = c.Get (3)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn4 = c.Get (4)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn5 = c.Get (5)->GetObject<ConstantPositionMobilityModel> ();

  mn0->SetPosition (Vector ( 0.5, 1.0, 0  ));
  mn1->SetPosition (Vector ( 1.0, 1.0, 0  ));
  mn2->SetPosition (Vector ( 1.5, 1.3, 0  ));
  mn3->SetPosition (Vector ( 2.0, 1.0, 0  ));
  mn4->SetPosition (Vector ( 2.5, 1.0, 0  ));
  mn5->SetPosition (Vector ( 1.5, 0.7, 0  ));

  anim.UpdateNodeSize(c.Get(0)->GetId(), .1, .1);
  anim.UpdateNodeSize(c.Get(1)->GetId(), .1, .1);
  anim.UpdateNodeSize(c.Get(2)->GetId(), .1, .1);
  anim.UpdateNodeSize(c.Get(3)->GetId(), .1, .1);
  anim.UpdateNodeSize(c.Get(4)->GetId(), .1, .1);
  anim.UpdateNodeSize(c.Get(5)->GetId(), .1, .1);

  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  NS_LOG_INFO ("Create Applications.");
  //uint16_t port = 9;   // Discard port (RFC 863)

  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (i3i4.GetAddress(1), sinkPort));     // interface of n4
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApp = packetSinkHelper.Install( c.Get (4));

  sinkApp.Start (Seconds (0));
  sinkApp.Stop (simulationEndTime);

  // Randomize the start time between 0 and 1ms
  Ptr<UniformRandomVariable> uniformRv = CreateObject<UniformRandomVariable> ();
  uniformRv->SetStream (0);

  BulkSendHelper source ("ns3::TcpSocketFactory", sinkAddress);
  source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApp = source.Install (c.Get (0));

  //sourceApp.Start (Seconds (0));
  //int start_time = uniformRv->GetInteger (0, 1000);

  //sourceApp.Start (MilliSeconds(start_time));
  int start_time = 1;
  sourceApp.Start(Seconds(start_time));
  sourceApp.Stop (simulationEndTime);

  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream (tcp_version + ".tr");
  p2p.EnableAsciiAll (stream);
  p2p.EnablePcapAll (tcp_version);

  NS_LOG_INFO("Time of start " << Seconds(start_time+0.00001));
  // Setup tracing for congestion window on node 0 and write results to file
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceCwnd,
                       prefix_file_name + "-cwnd.data", 0);
  // Setup tracing for RTT
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRtt,
                       prefix_file_name + "-rtt.data", 0);

  // Setup FlowMonitor and tracing for throughput
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceThroughput,
                       prefix_file_name + "-throughput.data", monitor);

  // Get node 1 and its ipv4
  Ptr<Node> n1 = c.Get (1);
  Ptr<Ipv4> n1ipv4 = n1->GetObject<Ipv4> ();
  // The first interfaceIndex is 0 for loopback, then the first p2p is numbered 1
  // The interface between node 1 and 2 has index 2:
  uint32_t n1ipv4ifIndex2 = 2;

  Simulator::Schedule (Seconds (start_time + 2.6), &Ipv4::SetDown, n1ipv4, n1ipv4ifIndex2);

  NS_LOG_INFO ("\nStarting Simulation Using TCP " + tcp_version + ".");
  Simulator::Stop(simulationEndTime);
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Simulation Done.");

  return 1;
}


int
main (int argc, char *argv[])
{
  // Allow the user to override any of the defaults and the above
  // Bind ()s at run-time, via command-line arguments
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  run("Bbr");
  run("Vegas");
  run("NewReno");
}