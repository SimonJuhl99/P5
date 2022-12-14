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

NS_LOG_COMPONENT_DEFINE ("two_routes");

uint32_t prevTx = 0;
uint32_t prevRx = 0;
Time prevTxTime = Seconds (0);
Time prevRxTime = Seconds (0);

static std::map<uint32_t, bool> firstCwnd;
static std::map<uint32_t, bool> firstRtt;
static std::map<uint32_t, bool> firstRto;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> cWndStream;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rttStream;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rtoStream;
static std::map<uint32_t, uint32_t> cWndValue;


static uint32_t
GetNodeIdFromContext (std::string context)
{
  std::size_t const n1 = context.find_first_of ("/", 1);
  std::size_t const n2 = context.find_first_of ("/", n1 + 1);
  return std::stoul (context.substr (n1 + 1, n2 - n1 - 1));
}

static void
ActivateError(Ptr<NetDevice> d, bool activate)
{
  Ptr<RateErrorModel> errorFree = CreateObject<RateErrorModel> ();
  Ptr<RateErrorModel> error = CreateObject<RateErrorModel> ();
  // an error rate of 1 means that error percentage is 100%
  errorFree->SetAttribute ("ErrorRate", DoubleValue (0));
  error->SetAttribute ("ErrorRate", DoubleValue (1));
  // if activate is true set error percentage to 100%
  if (activate) {
    d->SetAttribute ("ReceiveErrorModel", PointerValue (error));
  } else {
    d->SetAttribute ("ReceiveErrorModel", PointerValue (errorFree));
  }
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
TraceTxThroughput (std::string tp_tr_file_name, Ptr<FlowMonitor> monitor)
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
  thr <<  curTime << " " << 8 * (itr->second.txBytes - prevTx) / (1000 * 1000 * (curTime.GetSeconds () - prevTxTime.GetSeconds ())) << std::endl;
  // current time and current transmitted bytes which for next iteration becomes 'previous'
  prevTxTime = curTime;
  prevTx = itr->second.txBytes;
  // recursive call every x seconds
  Simulator::Schedule (Seconds (0.5), &TraceTxThroughput, tp_tr_file_name, monitor);
}

static void
TraceRxThroughput (std::string tp_tr_file_name, Ptr<FlowMonitor> monitor)
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
  thr <<  curTime << " " << 8 * (itr->second.rxBytes - prevRx) / (1000 * 1000 * (curTime.GetSeconds () - prevRxTime.GetSeconds ())) << std::endl;

  // thr <<  curTime << " " << itr->second.rxBytes << " " << prev  << std::endl;
  //<< i->second.rxBytes * 8.0 / simulationEndTime.GetSeconds () / 1000 / 1000  << " Mbps\n";
  // current time and current transmitted bytes which for next iteration becomes 'previous'
  prevRxTime = curTime;
  prevRx = itr->second.rxBytes;
  // recursive call every x seconds
  Simulator::Schedule (Seconds (0.5), &TraceRxThroughput, tp_tr_file_name, monitor);
}


int
main (int argc, char *argv[])
{

  uint32_t maxBytes = 0;
  std::string transportProtocol = "ns3::TcpNewReno";
  std::string prefix_file_name = "two-routes";

  Time simulationEndTime = Seconds (80);
  DataRate bottleneckBandwidth ("1Mbps");
  Time bottleneckDelay = MilliSeconds (5);
  DataRate defaultDatarate = DataRate (1 * bottleneckBandwidth.GetBitRate ());
  Time defaultLinkDelay = MilliSeconds (5);

  //Config::SetDefault ("ns3::BulkSendApplication::SendSize", UintegerValue (100000));
  //Config::SetDefault ("ns3::BulkSendApplication::DataRate", StringValue ("448kb/s"));
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transportProtocol)));
  //Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (10));

  LogComponentEnable("two_routes", LOG_LEVEL_ALL);

  // Allow the user to override any of the defaults and the above
  // Bind ()s at run-time, via command-line arguments
  CommandLine cmd (__FILE__);
  // cmd.AddValue ("transport_prot", "Transport protocol to use: TcpNewReno, TcpLinuxReno, "
  //               "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
  //               "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat, "
	// 	"TcpLp, TcpDctcp, TcpCubic, TcpBbr", transport_prot);

  cmd.Parse (argc, argv);

  // transport_prot = std::string ("ns3::") + transport_prot;

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

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (defaultDatarate));
  p2p.SetChannelAttribute ("Delay", TimeValue (defaultLinkDelay));
  NetDeviceContainer d0d1 = p2p.Install (n0n1);

  NetDeviceContainer d1d2 = p2p.Install (n1n2);

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (bottleneckBandwidth));
  p2p.SetChannelAttribute ("Delay", TimeValue (bottleneckDelay));
  NetDeviceContainer d2d3 = p2p.Install (n2n3);

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (defaultDatarate));
  p2p.SetChannelAttribute ("Delay", TimeValue (defaultLinkDelay));
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
  AnimationInterface anim ("two-routes.xml");

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


  BulkSendHelper source ("ns3::TcpSocketFactory", sinkAddress);
  source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApp = source.Install (c.Get (0));

  int start_time = 1;
  sourceApp.Start(Seconds(start_time));
  sourceApp.Stop (simulationEndTime);

  //AsciiTraceHelper ascii;
  //Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("two-routes.tr");
  //p2p.EnableAsciiAll (stream);
  p2p.EnablePcapAll ("two-routes");

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
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceTxThroughput,
                       prefix_file_name + "-tx-throughput.data", monitor);
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRxThroughput,
                       prefix_file_name + "-rx-throughput.data", monitor);

  // Get node 1 and its ipv4
  Ptr<Node> n1 = c.Get (1);
  Ptr<Ipv4> n1ipv4 = n1->GetObject<Ipv4> ();
  // The first interfaceIndex is 0 for loopback, then the first p2p is numbered 1
  // The interface between node 1 and 2 has index 2:
  uint32_t n1ipv4ifIndex2 = 2;
  uint32_t n1ipv4ifIndex3 = 3;

  Simulator::Schedule (Seconds (start_time + 0.00001), &Ipv4::SetDown, n1ipv4, n1ipv4ifIndex3);

  Simulator::Schedule (Seconds (start_time + 70), &Ipv4::SetUp, n1ipv4, n1ipv4ifIndex3);
  Simulator::Schedule (Seconds (start_time + 70), &Ipv4::SetDown, n1ipv4, n1ipv4ifIndex2);
  Simulator::Schedule (Seconds (start_time + 70), &ActivateError, d1d2.Get (1), true);

  // Simulator::Schedule (Seconds (start_time + 6), &ActivateError, d1d2.Get (1), false);
  // Simulator::Schedule (Seconds (start_time + 6), &Ipv4::SetUp, n1ipv4, n1ipv4ifIndex2);
  // Simulator::Schedule (Seconds (start_time + 6), &Ipv4::SetDown, n1ipv4, n1ipv4ifIndex3);
  // Simulator::Schedule (Seconds (start_time + 6), &ActivateError, d1d5.Get (1), true);
  //
  // Simulator::Schedule (Seconds (start_time + 9), &ActivateError, d1d5.Get (1), false);
  // Simulator::Schedule (Seconds (start_time + 9), &Ipv4::SetUp, n1ipv4, n1ipv4ifIndex3);
  // Simulator::Schedule (Seconds (start_time + 9), &Ipv4::SetDown, n1ipv4, n1ipv4ifIndex2);
  // Simulator::Schedule (Seconds (start_time + 9), &ActivateError, d1d2.Get (1), true);

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(simulationEndTime);
  Simulator::Run ();

  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

  //std::ostringstream os;

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

      std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
      std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
      std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
      std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / simulationEndTime.GetSeconds () / 1000 / 1000  << " Mbps\n";
      std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
      std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / simulationEndTime.GetSeconds () / 1000 / 1000  << " Mbps\n";
      //i->second.flowInterruptionsHistogram.SerializeToXmlStream(os, 2, "flowInterruptionsHistogram");
      //std::cout << "  Flow Interruptions Histogram:\n" << i->second.flowInterruptionsHistogram << "\n------------------\n";
    }

  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
