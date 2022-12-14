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
  // ofstream is an output stream class to operate on files
  // bitwise 'or' on either allowing output or appending to a stream
  std::ofstream thr (tp_tr_file_name, std::ios::out | std::ios::app);
  // write to stream:
  // current time and
  // transmitted bytes since last transmission divided by
  // the time since last transmission
  // which equals throughput (transmitted data over time)


  // NS_LOG_INFO ("Previous TX; " + std::to_string(prevTx));
  // NS_LOG_INFO ("Combined TX: " + std::to_string(itr->second.txBytes));
  // int current = itr->second.txBytes - prevTx;
  // NS_LOG_INFO ("Current TX: " + std::to_string(current) + "\n");


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
TraceRxThroughput (std::string tp_tr_file_name, Ptr<FlowMonitor> monitor)
{
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  // stats = monitor->GetFlowStats ();
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

  // Simulator::Schedule (Seconds (throughputTraceInterval), &TraceRxThroughput, tp_tr_file_name, monitor);
  Simulator::Schedule (Seconds (0.5), &TraceRxThroughput, tp_tr_file_name, monitor);
}


int emptyTraceFiles(string tcp_version)
{
  std::ofstream rtt (tcp_version + "-rtt.data", std::ios::out);
  std::ofstream cwnd (tcp_version + "-cwnd.data", std::ios::out);
  std::ofstream in (tcp_version + "-rx-throughput.data", std::ios::out);
  std::ofstream out (tcp_version + "-tx-throughput.data", std::ios::out);
  rtt <<  "";
  cwnd <<  "";
  in <<  "";
  out <<  "";

  return 1;
}

int resetTracingVars()
{
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();   // Pull out current package stats
  // stats = monitor->GetFlowStats ();   // Pull out current package stats
  auto itr = stats.begin ();

  //  NS-3 doesn't reset these variables, so set to the final state
  prevTx = itr->second.txBytes;
  prevRx = itr->second.rxBytes;
  prevTxTime = Now ();
  prevRxTime = Now ();

  return 1;
}