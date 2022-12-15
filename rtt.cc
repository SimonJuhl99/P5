/*      --  Roundtrip Script --
//  ----------------------------------------------
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


/*      --  Out-of-Order Script --
//  ----------------------------------
    Network nodes: 21.142.213 satellites (COME ON ELON!, WHAT 'U GOT?)
    Network template: SKRIV DET!

    -------------------------
// - Default Network Topology - \\


                           +-- n5 --+
                          /          \
            (8Mbps, 5ms) /            \ (8Mbps, 5ms)
                        /              \
      (8Mbps, 5ms)     /                \     (8Mbps, 5ms)
  n0 --------------- n1                  n3 --------------- n4
                       \                /
                        \              /
            (2Mbps, 5ms) \            / (8Mbps, 5ms)
                          \          /
                           +-- n2 --+
*/



// REPLACE WITH A REAL SCRIPT!!!
//
// KEEP AND UPDATE THE TOP COMMENT/EXPLANATION


///////////////////////////////////////////
/////  --  Placeholder Code  --
//  NS-3 need "functioning code" for parser-files to be recognized
///////////

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstddef>
#include <array>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("rtt");

uint32_t prevTx = 0;
uint32_t prevRx = 0;
Time prevTxTime = Seconds (0);
Time prevRxTime = Seconds (0);

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

// static void
// ActivateError(Ptr<NetDevice> d, bool activate)
// {
//   Ptr<RateErrorModel> errorFree = CreateObject<RateErrorModel> ();
//   Ptr<RateErrorModel> error = CreateObject<RateErrorModel> ();
//   // an error rate of 1 means that error percentage is 100%
//   errorFree->SetAttribute ("ErrorRate", DoubleValue (0));
//   error->SetAttribute ("ErrorRate", DoubleValue (1));
//   // if activate is true set error percentage to 100%
//   if (activate) {
//     d->SetAttribute ("ReceiveErrorModel", PointerValue (error));
//   } else {
//     d->SetAttribute ("ReceiveErrorModel", PointerValue (errorFree));
//   }
// }

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

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
  size_t pos = txt.find( ch );
  size_t initialPos = 0;
  strs.clear();
  // Decompose statement
  while( pos != std::string::npos ) {
    strs.push_back( txt.substr( initialPos, pos - initialPos ) );
    initialPos = pos + 1;
    pos = txt.find( ch, initialPos );
  }
  // Add the last one
  strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );
  return strs.size();
}

void ChangeDataRate(Ptr<PointToPointNetDevice> d0, Ptr<PointToPointNetDevice> d1, Ptr<PointToPointChannel> c, double dr)
{
  std::string dr_string = std::to_string(dr) + "Mbps";
  d0->SetDataRate(DataRate(dr_string));
  d1->SetDataRate(DataRate(dr_string));
}

void ChangePropDelay(Ptr<PointToPointChannel> c, double delay)
{
  c->SetDelay(MilliSeconds(delay));
}

PointToPointHelper::PointToPointHelper ()
{
  m_queueFactory.SetTypeId ("ns3::DropTailQueue<Packet>");
  m_deviceFactory.SetTypeId ("ns3::PointToPointNetDevice");
  m_channelFactory.SetTypeId ("ns3::PointToPointChannel");
  m_enableFlowControl = true;
}

auto
PointToPointHelper::InstallWithoutContainer(Ptr<Node> a, Ptr<Node> b) {

  Ptr<PointToPointNetDevice> devA = m_deviceFactory.Create<PointToPointNetDevice> ();
  devA->SetAddress (Mac48Address::Allocate ());
  a->AddDevice (devA);
  Ptr<Queue<Packet> > queueA = m_queueFactory.Create<Queue<Packet> > ();
  devA->SetQueue (queueA);
  Ptr<PointToPointNetDevice> devB = m_deviceFactory.Create<PointToPointNetDevice> ();
  devB->SetAddress (Mac48Address::Allocate ());
  b->AddDevice (devB);
  Ptr<Queue<Packet> > queueB = m_queueFactory.Create<Queue<Packet> > ();
  devB->SetQueue (queueB);
  if (m_enableFlowControl)
    {
      // Aggregate NetDeviceQueueInterface objects
      Ptr<NetDeviceQueueInterface> ndqiA = CreateObject<NetDeviceQueueInterface> ();
      ndqiA->GetTxQueue (0)->ConnectQueueTraces (queueA);
      devA->AggregateObject (ndqiA);
      Ptr<NetDeviceQueueInterface> ndqiB = CreateObject<NetDeviceQueueInterface> ();
      ndqiB->GetTxQueue (0)->ConnectQueueTraces (queueB);
      devB->AggregateObject (ndqiB);
    }

  Ptr<PointToPointChannel> channel = 0;
  channel = m_channelFactory.Create<PointToPointChannel> ();

  devA->Attach (channel);
  devB->Attach (channel);

  struct retVals {
    Ptr<PointToPointNetDevice> i1, i2;
    Ptr<PointToPointChannel> i3;
  };

  return retVals {devA, devB, channel};
  // return netDevVector;
}

int
main (int argc, char *argv[])
{

  uint32_t maxBytes = 0;
  uint32_t speed_of_light = 299792458;
  std::string transportProtocol = "ns3::TcpNewReno";
  std::string prefix_file_name = "rtt";

  uint32_t intSimulationEndTime = 700;
  Time simulationEndTime = Seconds (intSimulationEndTime);
  DataRate linkBandwidth ("1Mbps");
  Time linkDelay = MilliSeconds (5);

  //Config::SetDefault ("ns3::BulkSendApplication::SendSize", UintegerValue (100000));
  //Config::SetDefault ("ns3::BulkSendApplication::DataRate", StringValue ("448kb/s"));
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transportProtocol)));
  Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (10));

  LogComponentEnable("rtt", LOG_LEVEL_ALL);

  CommandLine cmd (__FILE__);
  // cmd.AddValue ("transport_prot", "Transport protocol to use: TcpNewReno, TcpLinuxReno, "
  //               "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
  //               "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat, "
	// 	"TcpLp, TcpDctcp, TcpCubic, TcpBbr", transport_prot);

  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (2);

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c);

  InternetStackHelper internet;
  internet.Install (c);

  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (linkBandwidth));
  p2p.SetChannelAttribute ("Delay", TimeValue (linkDelay));
  // NetDeviceContainer d = p2p.Install (c);

  // std::vector<PointToPointNetDevice> netDevices = p2p.InstallWithoutContainer(c.Get(0), c.Get(1));

  // eyo.SetDataRate(DataRate("1Mbps"));



  auto [d0, d1, channel] = p2p.InstallWithoutContainer(c.Get(0), c.Get(1));

  NetDeviceContainer d;
  d.Add(d0);
  d.Add(d1);

  // NS_LOG_INFO (channel->GetDelay());
  // channel->SetDelay(MilliSeconds (5));
  // NS_LOG_INFO (channel->GetDevice(0));
  //
  // d0->SetDataRate(DataRate("1Mbps"));
  // d1->SetDataRate(DataRate("1Mbps"));

  // We add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interface = ipv4.Assign (d);     // need container to get address more easily later on

  // Create the animation object and configure for specified output
  AnimationInterface anim ("rtt.xml");

  Ptr<ConstantPositionMobilityModel> mn0 = c.Get (0)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> mn1 = c.Get (1)->GetObject<ConstantPositionMobilityModel> ();

  mn0->SetPosition (Vector ( 0.5, 1.0, 0  ));
  mn1->SetPosition (Vector ( 1.0, 1.0, 0  ));

  anim.UpdateNodeSize(c.Get(0)->GetId(), .1, .1);
  anim.UpdateNodeSize(c.Get(1)->GetId(), .1, .1);

    // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (interface.GetAddress(1), sinkPort));     // interface of n4
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApp = packetSinkHelper.Install( c.Get (1));

  sinkApp.Start (Seconds (0));
  sinkApp.Stop (simulationEndTime);

  BulkSendHelper source ("ns3::TcpSocketFactory", sinkAddress);
  source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApp = source.Install (c.Get (0));

  int start_time = 1;
  sourceApp.Start(Seconds(start_time));
  sourceApp.Stop (simulationEndTime);

  // AsciiTraceHelper ascii;
  // Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("rtt.tr");
  // p2p.EnableAsciiAll (stream);
  // p2p.EnablePcapAll ("rtt");


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

  int oppo_len = 772;
  int parallel_len = 2901;

  std::string oppo_dr_arr[oppo_len + 1];
  std::string oppo_dist_arr[oppo_len + 1];
  std::string parallel_dr_arr[parallel_len + 1];
  std::string parallel_dist_arr[parallel_len + 1];


  std::string mystring;
  std::fstream myfile;
  myfile.open("oppo.data");

  int i = 0;
  std::vector<std::string> v;

  if ( myfile.is_open() ) { // always check whether the file is open
    while(!myfile.eof()) {
      getline(myfile, mystring);
      split(mystring, v, ' ');
      if (v.size() > 1) {
        // std::cout << v.at(2) << "\n";
        oppo_dr_arr[i]=v.at(1);
        oppo_dist_arr[i]=v.at(2);
      }
      i++;
    }
    myfile.close();
  }

  for(int i = 0; i < oppo_len ; i++) {
    int int_converted_vals = std::stoi(oppo_dr_arr[i]);
    double megabits_per_sec = (double)int_converted_vals / (double)1000000;
    double dummy_datarate = megabits_per_sec / 1000.0;
    double prop_delay = (double)std::stoi(oppo_dist_arr[i]) / (double)speed_of_light;
    Simulator::Schedule (Seconds (i), &ChangeDataRate, d0, d1, channel, dummy_datarate);
    Simulator::Schedule (Seconds (i), &ChangePropDelay, channel, prop_delay);
  }
  // Simulator::Schedule (Seconds (1), &ChangeDataRate, channel, 0.342);


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(simulationEndTime);
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
