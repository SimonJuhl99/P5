// Tracing Variables

// Congestion Window Variables
static std::map<uint32_t, bool> firstCwnd;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> cWndStream;
static std::map<uint32_t, uint32_t> cWndValue;
// Roundtrip Time Variables
static std::map<uint32_t, bool> firstRtt;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rttStream;
// Timeout Variables
static std::map<uint32_t, bool> firstRto;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rtoStream;
// Throughput Variables
uint32_t prevTx = 0;    // Earlier values for throughput tracing
uint32_t prevRx = 0;    // Earlier values for throughput tracing
Time prevTxTime = Seconds (0);    // Earlier timestamps for throughput tracing
Time prevRxTime = Seconds (0);    // Earlier timestamps for throughput tracing

uint32_t prevTx1 = 0;    // Earlier values for throughput tracing
uint32_t prevTx2 = 0;    // Earlier values for throughput tracing
uint32_t prevRx1 = 0;    // Earlier values for throughput tracing
uint32_t prevRx2 = 0;    // Earlier values for throughput tracing
Time prevTx1Time = Seconds (0);    // Earlier timestamps for throughput tracing
Time prevTx2Time = Seconds (0);    // Earlier timestamps for throughput tracing
Time prevRx1Time = Seconds (0);    // Earlier timestamps for throughput tracing
Time prevRx2Time = Seconds (0);    // Earlier timestamps for throughput tracing

FlowMonitorHelper flowmon;    // Not used before network setup
Ptr<FlowMonitor> monitor;     // Not used before network setup

// Trace EVERYTHING variable
// AsciiTraceHelper ascii;       // Not used before network setup

// FlowMonitor::FlowStatsContainer stats;


static uint32_t
GetNodeIdFromContext (string context)
{
  std::size_t const n1 = context.find_first_of ("/", 1);
  std::size_t const n2 = context.find_first_of ("/", n1 + 1);
  return std::stoul (context.substr (n1 + 1, n2 - n1 - 1));
}


static void
CwndTracer (string context,  uint32_t oldval, uint32_t newval)
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
TraceCwnd(std::string cwnd_tr_file_name, uint32_t nodeId)
{
  AsciiTraceHelper ascii;
  cWndStream[nodeId] = ascii.CreateFileStream(cwnd_tr_file_name);
  Config::Connect("/NodeList/" + std::to_string(nodeId) +
                      "/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
                  MakeCallback(&CwndTracer));
}

// static void
// TraceCwnd (string cwnd_tr_file_name, uint32_t nodeId)
// {
//   //NS_LOG_INFO("TraceCwnd Start");
//   AsciiTraceHelper ascii;
//   cWndStream[nodeId] = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
//   Config::Connect ("/NodeList/" + std::to_string (nodeId) + "/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
//                    MakeCallback (&CwndTracer));
// }

static void
RttTracer (string context, Time oldval, Time newval)
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
TraceRtt (string rtt_tr_file_name, uint32_t nodeId)
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
  // stats = monitor->GetFlowStats ();
  // pointer to first value in stats
  auto itr = stats.begin ();
  // current time
  Time curTime = Now ();


  std::ofstream thr (tp_tr_file_name + ".data", std::ios::out | std::ios::app);


  // thr <<  curTime << " " << 8 * (current) / (1000 * 1000 * (curTime.GetSeconds () - prevTxTime.GetSeconds ())) << std::endl;
  thr <<  curTime << " " << 8 * (itr->second.txBytes - prevTx) / (1000 * 1000 * (curTime.GetSeconds () - prevTxTime.GetSeconds ())) << std::endl;
  // current time and current transmitted bytes which for next iteration becomes 'previous'
  prevTxTime = curTime;
  prevTx = itr->second.txBytes;
  // recursive call every x seconds

  // Simulator::Schedule (Seconds (throughputTraceInterval), &TraceTxThroughput, tp_tr_file_name, monitor);
  Simulator::Schedule (Seconds (0.5), &TraceTxThroughput, tp_tr_file_name, monitor);
}

static void
TraceConTxThroughput (std::string tp_tr_file_name, Ptr<FlowMonitor> monitor)
{
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  Time curTime = Now ();

  std::ofstream thr1 (tp_tr_file_name + "-1.data", std::ios::out | std::ios::app);
  std::ofstream thr2 (tp_tr_file_name + "-2.data", std::ios::out | std::ios::app);

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      if (i->first == 1) {
        thr1 << curTime << " " << 8 * (i->second.txBytes - prevTx1) / (1000 * 1000 * (curTime.GetSeconds () - prevTx1Time.GetSeconds ())) << std::endl;
        prevTx1Time = curTime;
        prevTx1 = i->second.txBytes;
      } else if (i->first == 2) {
        thr2 << curTime << " " << 8 * (i->second.txBytes - prevTx2) / (1000 * 1000 * (curTime.GetSeconds () - prevTx2Time.GetSeconds ())) << std::endl;
        prevTx2Time = curTime;
        prevTx2 = i->second.txBytes;
      }
    }

  Simulator::Schedule (Seconds (0.5), &TraceConTxThroughput, tp_tr_file_name, monitor);
}

static void
TraceRxThroughput (std::string tp_tr_file_name, Ptr<FlowMonitor> monitor)
{
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  // stats = monitor->GetFlowStats ();
  // pointer to first value in stats
  auto itr = stats.begin ();
  // current time
  Time curTime = Now ();

  std::ofstream thr (tp_tr_file_name + ".data", std::ios::out | std::ios::app);

  thr <<  curTime << " " << 8 * (itr->second.rxBytes - prevRx) / (1000 * 1000 * (curTime.GetSeconds () - prevRxTime.GetSeconds ())) << std::endl;

  prevRxTime = curTime;
  prevRx = itr->second.rxBytes;

  Simulator::Schedule (Seconds (0.5), &TraceRxThroughput, tp_tr_file_name, monitor);
}

static void
TraceConRxThroughput (std::string tp_tr_file_name, Ptr<FlowMonitor> monitor)
{
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  Time curTime = Now ();

  std::ofstream thr1 (tp_tr_file_name + "-1.data", std::ios::out | std::ios::app);
  std::ofstream thr2 (tp_tr_file_name + "-2.data", std::ios::out | std::ios::app);

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      // cout << "Datarate: " << i->second.rxBytes << std::endl;
      if (i->first == 1) {
        thr1 << curTime << " " << 8 * (i->second.rxBytes - prevRx1) / (1000 * 1000 * (curTime.GetSeconds () - prevRx1Time.GetSeconds ())) << std::endl;
        prevRx1Time = curTime;
        prevRx1 = i->second.rxBytes;
      } else if (i->first == 2) {
        thr2 << curTime << " " << 8 * (i->second.rxBytes - prevRx2) / (1000 * 1000 * (curTime.GetSeconds () - prevRx2Time.GetSeconds ())) << std::endl;
        prevRx2Time = curTime;
        prevRx2 = i->second.rxBytes;
      }
    }

  Simulator::Schedule (Seconds (0.5), &TraceConRxThroughput, tp_tr_file_name, monitor);
}


int emptyTraceFiles(string tcp_version, string error, int extra_node = -1)
{

  if (extra_node > 0) {
    string node_file = "-" + std::to_string(extra_node);

    std::ofstream rtt (tcp_version + "-rtt" + node_file + error + ".data", std::ios::out);
    std::ofstream cwnd (tcp_version + "-cwnd" + node_file + error + ".data", std::ios::out);
  }

  std::ofstream rtt (tcp_version + "-rtt" + error + ".data", std::ios::out);
  std::ofstream cwnd (tcp_version + "-cwnd" + error + ".data", std::ios::out);
  std::ofstream in (tcp_version + "-rx-throughput" + error + ".data", std::ios::out);
  std::ofstream out (tcp_version + "-tx-throughput" + error + ".data", std::ios::out);
  std::ofstream drop (tcp_version + "-dropped" + error + ".data", std::ios::out);
  rtt <<  "";
  cwnd <<  "";
  in <<  "";
  out <<  "";
  drop <<  "";

  return 1;
}

int resetTracingVars(string tcp_version, string error)
{
  //  NS-3 doesn't reset these variables, so set to the final state
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  auto itr = stats.begin ();

  // prevTx = 0;    // Earlier values for throughput tracing
  // prevRx = 0;    // Earlier values for throughput tracing
  // prevTx1 = 0;
  // prevTx2 = 0;
  // prevRx1 = 0;
  // prevRx2 = 0;


  prevRx = itr->second.rxBytes;    // Earlier values for throughput tracing
  prevTx = itr->second.txBytes;    // Earlier values for throughput tracing


  std::ofstream drop (tcp_version + "-dropped" + error + ".data", std::ios::out | std::ios::app);


  // thr <<  curTime << " " << 8 * (current) / (1000 * 1000 * (curTime.GetSeconds () - prevTxTime.GetSeconds ())) << std::endl;
  drop << "Tx:"<< 8 * prevTx << " Rx:" << 8 * prevRx << " Dropped:" << 8 * (prevTx - prevRx) << std::endl;

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i) 
  {
    if (i->first == 1) {
      prevRx1 = i->second.rxBytes;
    } else if (i->first == 2) {
      prevRx2 = i->second.rxBytes;
    }
  }




  // itr++;
  // prevRx1 = itr->second.rxBytes;
  // prevTx1 = itr->second.txBytes;
  // itr++;
  // prevRx2 = itr->second.rxBytes;
  // prevTx2 = itr->second.txBytes;




  prevTxTime = Now ();    // Earlier timestamps for throughput tracing
  prevRxTime = Now ();    // Earlier timestamps for throughput tracing
  prevTx1Time = Now ();
  prevTx2Time = Now ();
  prevRx1Time = Now ();
  prevRx2Time = Now ();


  return 1;
}
